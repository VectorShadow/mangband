/*
 * Support for the "remote console".  It lets the server admin
 * perform server maintenance without requiring that the server
 * have a devoted tty.
 */

#include "angband.h"

static sockbuf_t console_buf;
static bool console_authenticated;
static bool console_listen;

/*
 * Output some text to the console, if we are listening
 */
void console_print(char *msg)
{
	if (console_listen)
	{
		Packet_printf(&console_buf, "%s%c",msg,'\n');
		Sockbuf_flush(&console_buf);
	}
}

/*
 * Return the list of players
 */
static void console_who()
{
	int k;

	/* Packet header */
	Packet_printf(&console_buf, "%s",format("%d players online\n", NumPlayers));
	
	/* Scan the player list */
	for (k = 1; k <= NumPlayers; k++)
	{
		player_type *p_ptr = Players[k];

		/* Add an entry */
		Packet_printf(&console_buf, "%s",format("%s is a level %d %s %s at %d ft\n", 
			p_ptr->name, p_ptr->lev, race_info[p_ptr->prace].title,
			class_info[p_ptr->pclass].title, p_ptr->dun_depth*50));
			
	}
	Sockbuf_flush(&console_buf);
}

/*
 * Return information about a specific player
 */
static void console_whois(char *name)
{
	int i, len;
	char output[1024];
	player_type *p_ptr, *p_ptr_search;
	
	p_ptr = 0;

	/* Find this player */
	for (i = 1; i <= NumPlayers; i++)
	{
		p_ptr_search = Players[i];
		len = strlen(p_ptr_search->name);
		if (!strncasecmp(p_ptr_search->name, name, len))
		{
			p_ptr = p_ptr_search;
		}
	}
	if (!p_ptr)
	{
		Packet_printf(&console_buf, "%s%c","No such player",'\n');
		Sockbuf_flush(&console_buf);
		return;
	}
	
	/* Output player information */

	/* General character description */
	Packet_printf(&console_buf, "%s",format("%s is a level %d %s %s at %d ft\n", 
		p_ptr->name, p_ptr->lev, race_info[p_ptr->prace].title,
		class_info[p_ptr->pclass].title, p_ptr->dun_depth*50));
	
	/* Player connection info */
	Packet_printf(&console_buf, "%s",format("(%s@%s [%s] v%d)\n", 
		p_ptr->realname, p_ptr->hostname, p_ptr->addr, p_ptr->version));
				
	/* Other interesting factoids */
	if ( p_ptr->lives > 0 )
		Packet_printf(&console_buf, "%s",format("%s resurected %d times.\n", p_ptr->lives));
	if ( p_ptr->max_dlv == 0 )
		Packet_printf(&console_buf, "%s",format("Has never left the town!\n"));
	else
		Packet_printf(&console_buf, "%s",format("Has ventured down to %d ft\n", p_ptr->max_dlv*50));
	i = p_ptr->msg_hist_ptr-1;
	if( i >= 0 )
	{
		if (p_ptr->msg_log[i])
		{
			Packet_printf(&console_buf, "%s",format("Last message: %s\n", p_ptr->msg_log[i]));
		}
	}
		

	Sockbuf_flush(&console_buf);
	
}

static void console_message(char *buf)
{
	/* Send the message */
	player_talk(0, buf);
}

static void console_kick_player(char *name)
{
	int i;

	/* Check the players in the game */
	for (i = 1; i <= NumPlayers; i++)
	{
		/* Check name */
		if (!strcmp(name, Players[i]->name))
		{
			/* Kick him */
			Destroy_connection(Players[i]->conn, "kicked out");

			/* Success */
			Packet_printf(&console_buf, "%s", "Kicked player\n");
			Sockbuf_flush(&console_buf);
			return;
		}
	}

	/* Failure */
	Packet_printf(&console_buf, "%s", "No such player\n");
	Sockbuf_flush(&console_buf);

}

/*
 * Test the integrity of the RNG
 */
static void console_rng_test()
{
	int i;
	u32b outcome, value;
	/* This is the expected outcome, generated on our reference platform */
	u32b reference = 0x0D3E5371;
	
	bool randquick = Rand_quick;
	u32b randvalue = Rand_value;
	u16b randplace = Rand_place;
	u32b randstate[RAND_DEG];

	/* Don't run this if any players are connected */
	if(NumPlayers > 0)
	{
		Packet_printf(&console_buf, "%s", "Can't run the RNG test with players connected!\n");
		Sockbuf_flush(&console_buf);
		return;
	}
	
	/* Preserve current RNG state */
	for( i=0; i<RAND_DEG; i++ ) randstate[i] = Rand_state[i];

	/* Initialise to a known state */
	Rand_quick = FALSE;
	Rand_value = 0;
	Rand_place = 0;
	Rand_state_init(0xDEADDEAD);
	outcome = 0;

	/* Let the operator know we are busy */
	Packet_printf(&console_buf, "%s", "Torturing the RNG for 100 million iterations...\n");
	Sockbuf_flush(&console_buf);

	/* Torture the RNG for a hundred million iterations */
	for(i=0;i<100000000;i++)
	{
		/* Flip between the quick and the complex */
		Rand_quick = (i % 2);
		outcome ^= Rand_mod(0x0FFFFFFF);
		outcome ^= Rand_div(0x0FFFFFFF);	
	}

	/* Display the results */
	if(outcome == reference)
	{
		Packet_printf(&console_buf, "%s","RNG is working perfectly\n");
	} else {
		Packet_printf(&console_buf, "%s","RNG integrity check FAILED\n");
		Packet_printf(&console_buf, "%s",
			format("Outcome was 0x%08X, expected 0x%08X\n",outcome, reference));
	}
	Sockbuf_flush(&console_buf);
	
	/* Restore the RNG state */
	Rand_quick = randquick;
	Rand_value= randvalue;
	Rand_place = randplace;
	for( i=0; i<RAND_DEG; i++ ) Rand_state[i] = randstate[i];
}

static void console_reload_server_preferences(void)
{
	/* Reload the server preferences */
	load_server_cfg();

	/* Let mangconsole know that the command was a success */
	/* Packet header */
	Packet_printf(&console_buf, "%s", "Reloaded\n");

	/* Write the output */
	DgramReply(console_buf.sock, console_buf.ptr, console_buf.len);
}

static void console_shutdown(void)
{
	/* Packet header */
	Packet_printf(&console_buf, "%s", "Server shutdown\n");

	/* Write the output */
	Sockbuf_flush(&console_buf);

	/* Shutdown */
	shutdown_server();
}

/*
 * This is the response function when incoming data is received on the
 * control pipe.
 */
void NewConsole(int read_fd, int arg)
{
	char ch, passwd[80], buf[1024];
	char *params;
	int i, j, bytes, buflen;
	static int newsock = 0;

	/* Make a TCP connection */
	/* Hack -- check if this data has arrived on the contact socket or not.
	 * If it has, then we have not created a connection with the client yet, 
	 * and so we must do so.
	 */
	if (read_fd == ConsoleSocket)
	{
		// Hack -- make sure that two people haven't tried to use mangconsole
		// at the same time.  Since I am currently too lazy to support this,
		// we will remove the input of the first person when the second person
		// connects.
		if (newsock) remove_input(newsock);
		if ((newsock = SocketAccept(read_fd)) == -1)
		{
			quit("Couldn't accept TCP connection.\n");
		}
		console_buf.sock = newsock;
		install_input(NewConsole, newsock, 2);
		console_authenticated = FALSE;
		console_listen = FALSE;
		Packet_printf(&console_buf, "%s","Connected\n");
		Sockbuf_flush(&console_buf);

		return;
	}


	/* Clear the buffer */
	Sockbuf_clear(&console_buf);
	/* Read the message */
	bytes = DgramReceiveAny(read_fd, console_buf.buf, console_buf.size);

	/* Check for errors or our TCP connection closing */
	if (bytes <= 0)
	{
		/* If this happens our TCP connection has probably been severed.
		 * Remove the input.
		 */
		//s_printf("Error reading from console socket\n");
		remove_input(newsock);
		newsock = 0;

		return;
	}

	/* Set length */
	console_buf.len = bytes;

	/* Acquire sender's address */
//	strcpy(host_name, DgramLastname()); 

	/* Get the password if not authenticated */
	if(!console_authenticated)
	{
		Packet_scanf(&console_buf, "%N",passwd); 

		/* Check for illegal accesses */
		if (strcmp(passwd, cfg_console_password))
		{
			/* Clear buffer */
			Sockbuf_clear(&console_buf);
	
			/* Put an "illegal access" reply in the buffer */
			Packet_printf(&console_buf, "%s", "Invalid password\n");
			
			/* Send it */
			DgramWrite(read_fd, console_buf.buf, console_buf.len);

			/* Log this to the local console */
			s_printf("Illegal console command from %s.\n", DgramLastname());

			return;
		}
		else 
		{
			/* Clear buffer */
			Sockbuf_clear(&console_buf);
			console_authenticated = TRUE;
			Packet_printf(&console_buf, "%s","Authenticated\n");
			Sockbuf_flush(&console_buf);
		}
	}

	/* Acquire command in the form: <command> <params> */
	Packet_scanf(&console_buf, "%N", buf);
	buflen = strlen(buf);

	/* Split up command and params */
	if(params = strstr(buf," "))
	{
		*params++ = '\0';
	}
	else
	{
		params = buf;
	}

	/* Clear buffer */
	Sockbuf_clear(&console_buf);

	/* Determine what the command is */
	if (!strncmp(buf,"listen",6)) 
	{
		console_listen = TRUE;
	}
	else if (!strncmp(buf,"who",6)) 
	{
		console_who();
	}
	else if (!strncmp(buf,"shutdown",8))
	{
		console_shutdown();
	}
	else if (!strncmp(buf,"msg",3))
	{
		console_message(params);
	}
	else if (!strncmp(buf,"kick",4))
	{
		console_kick_player(params);
	}
	else if (!strncmp(buf,"reload",6))
	{
		console_reload_server_preferences();
	}
	else if (!strncmp(buf,"whois",5))
	{
		console_whois(params);
	}
	else if (!strncmp(buf,"rngtest",7))
	{
		console_rng_test();
	}
	
}

/*
 * Initialize the stuff for the new console
 */
bool InitNewConsole(int write_fd)
{
	/* Initialize buffer */
	if (Sockbuf_init(&console_buf, write_fd, 8192, SOCKBUF_READ | SOCKBUF_WRITE))
	{
		/* Failed */
		s_printf("No memory for console buffer.\n");

		return FALSE;
	}

	return TRUE;
}

