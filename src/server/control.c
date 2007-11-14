/*
 * Support for the "remote console".  It lets the server admin
 * perform server maintenance without requiring that the server
 * have a devoted tty.
 */

#include "angband.h"

#ifdef NEW_SERVER_CONSOLE

static sockbuf_t console_buf;

/*
 * Return the list of players
 */
static void console_status()
{
	int k;

	/* Packet header */
	Packet_printf(&console_buf, "%c%d", CONSOLE_STATUS, NumPlayers);

	/* Scan the player list */
	for (k = 1; k <= NumPlayers; k++)
	{
		player_type *p_ptr = Players[k];

#if 0
		/* Skip disconnected players */
		if (p_ptr->conn == NOT_CONNECTED) continue;
#endif

		/* Add an entry */
		Packet_printf(&console_buf, "%s%s%s%d%d",
			p_ptr->name, race_info[p_ptr->prace].title,
			class_info[p_ptr->pclass].title, p_ptr->lev,
			p_ptr->dun_depth);
	}
}

static void console_player_info(int player)
{
}

static void console_artifacts()
{
	int i, count = 0, z;
	bool okay[MAX_A_IDX];
	char base_name[80];

	/* Packet header */
	Packet_printf(&console_buf, "%c", CONSOLE_ARTIFACT_LIST);

	/* Scan the artifacts */
	for (i = 0; i < MAX_A_IDX; i++)
	{
		artifact_type *a_ptr = &a_info[i];

		/* Default */
		okay[i] = FALSE;

		/* Skip "empty" artifacts */
		if (!a_ptr->name) continue;

		/* Skip "uncreated" artifacts */
		if (!a_ptr->cur_num) continue;

		/* Assume OK */
		okay[i] = TRUE;

		/* One more artifact */
		count++;
	}

	/* Write the number */
	Packet_printf(&console_buf, "%d", count);

	/* Write each artifact info */
	for (i = 0; i < MAX_A_IDX; i++)
	{
		artifact_type *a_ptr = &a_info[i];

		/* List "dead" ones */
		if (!okay[i]) continue;

		/* Paranoia */
		strcpy(base_name, "Unknown Artifact");

		/* Obtain the base object type */
		z = lookup_kind(a_ptr->tval, a_ptr->sval);

		/* Real object */
		if (z)
		{
			object_type forge;

			/* Create the object */
			invcopy(&forge, z);

			/* Create the artifact */
			forge.name1 = i;

			/* Describe the artifact */
			object_desc_store(0, base_name, &forge, FALSE, 0);
		}

		/* Output this artifact's number and name */
		Packet_printf(&console_buf, "%d%s", i, base_name);
	}
}

static void console_uniques()
{
	int k, count = 0;
	char buf[1024], tmp[1024];

	/* Packet header */
	Packet_printf(&console_buf, "%c", CONSOLE_UNIQUE_LIST);

	/* Scan the monster races */
	for (k = 1; k < MAX_R_IDX - 1; k++)
	{
		monster_race *r_ptr = &r_info[k];

		/* Only process uniques */
		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			bool dead = (r_ptr->max_num == 0);

			/* Only count known uniques */
			if (dead || r_ptr->r_sights)
			{
				/* One more */
				count++;
			}
		}
	}
	
	/* Write the number of uniques */
	Packet_printf(&console_buf, "%d", count);

	/* Scan the monster races */
	for (k = 1; k < MAX_R_IDX - 1; k++)
	{
		monster_race *r_ptr = &r_info[k];

		/* Only process uniques */
		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			/* Only display known uniques */
			if (r_ptr->r_sights)
			{
				int i;
				byte ok = FALSE;
			
				/* Format message */
				sprintf(buf, "%s has been killed by:\n");

				for (i = 1; i <= NumPlayers; i++)
				{
					player_type *q_ptr = Players[i];

					if (q_ptr->r_killed[k])
					{
						sprintf(buf, "        %s\n");
						ok = TRUE;
					}
				}
				if (!ok) sprintf(buf, "       Nobody\n");


				/* Add info */
				Packet_printf(&console_buf, "%d%s", k, buf);
			}
		}
	}
}

static void console_change_artifact(int artifact, int status)
{
	artifact_type *a_ptr;

	/* Check bounds */
	if (artifact <= 0 || artifact > MAX_A_IDX)
	{
		/* Failed */
		Packet_printf(&console_buf, "%c%c", CONSOLE_CHANGE_ARTIFACT, 0);

		return;
	}

	/* Set pointer */
	a_ptr = &a_info[artifact];

	/* Set the artifact's status */
	if (status)
	{
		/* Make found */
		a_ptr->cur_num = 1;
	}
	else
	{
		/* Make unfound */
		a_ptr->cur_num = 0;
	}

	/* Succeeded */
	Packet_printf(&console_buf, "%c%c", CONSOLE_CHANGE_ARTIFACT, 1);
}

static void console_change_unique(int unique, cptr killer)
{
	monster_race *r_ptr;
	int kill_idx;
	char buf[80];

	/* Check bounds */
	if (unique <= 0 || unique > MAX_R_IDX - 1)
	{
		/* Failed */
		Packet_printf(&console_buf, "%c%c", CONSOLE_CHANGE_UNIQUE, 0);

		return;
	}

	/* Get killer index */
	if (killer)
	{
		/* Lookup player by name */
		kill_idx = lookup_player_id(killer);
	}
	else
	{
		/* No killer */
		kill_idx = 0;
	}

	/* Set pointer */
	r_ptr = &r_info[unique];

	/* Check for uniqueness */
	if (!r_ptr->flags1 & RF1_UNIQUE)
	{
		/* Failed */
		Packet_printf(&console_buf, "%c%c", CONSOLE_CHANGE_UNIQUE, 0);
		
		return;
	}

	/* Set death flag */
	if (kill_idx)
	{
		/* Dead */
		r_ptr->max_num = 0;
        //r_ptr->killer = kill_idx;
	}
	else
	{
		/* Alive */

		/* Tell people if the monster is respawning */
		if (!r_ptr->max_num)
		{
    			sprintf(buf,"%s rises from the dead!",(r_name + r_ptr->name));
    			
    			/* Tell every player */
    			msg_broadcast(0,buf);    				    				
		}
		r_ptr->max_num = 1;
        //r_ptr->killer = 0;
	}

	/* Succeeded */
	Packet_printf(&console_buf, "%c%c", CONSOLE_CHANGE_UNIQUE, 1);
}

static void console_message(char *buf)
{
	/* Send the message */
	player_talk(0, buf);

	/* Reply */
	Packet_printf(&console_buf, "%c", CONSOLE_MESSAGE);
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
			Packet_printf(&console_buf, "%c%c", CONSOLE_KICK_PLAYER, 1);

			return;
		}
	}

	/* Failure */
	Packet_printf(&console_buf, "%c%c", CONSOLE_KICK_PLAYER, 0);
}

static void console_reload_server_preferences(void)
{
	/* Reload the server preferences */
	load_server_cfg();

	/* Let mangconsole know that the command was a success */
	/* Packet header */
	Packet_printf(&console_buf, "%c", CONSOLE_RELOAD_SERVER_PREFERENCES);

	/* Write the output */
	DgramReply(console_buf.sock, console_buf.ptr, console_buf.len);
}
static void console_shutdown(void)
{
	/* Packet header */
	Packet_printf(&console_buf, "%c", CONSOLE_SHUTDOWN);

	/* Write the output */
	DgramReply(console_buf.sock, console_buf.ptr, console_buf.len);

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
	int i, j, bytes;
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

	/* Get the password */
	Packet_scanf(&console_buf, "%s",passwd); 

	/* Check for illegal accesses */
	//if (console_bad_name(host_name))
	if (strcmp(passwd, cfg_console_password))
	{
		/* Clear buffer */
		Sockbuf_clear(&console_buf);

		/* Put an "illegal access" reply in the buffer */
		Packet_printf(&console_buf, "%c", CONSOLE_DENIED);
		
		/* Send it */
		DgramWrite(read_fd, console_buf.buf, console_buf.len);

		/* Log this to the local console */
		s_printf("Illegal console command from %s.\n", DgramLastname());

		return;
	}
	else s_printf("Valid console command from %s.\n", DgramLastname());

	/* Acquire command */
	Packet_scanf(&console_buf, "%c", &ch);

	/* Determine what the command is */
	switch(ch)
	{
		/* Wants to see the player list */
		case CONSOLE_STATUS:
			console_status();
			break;

		/* Wants to see detailed player info */
		case CONSOLE_PLAYER_INFO:
			Packet_scanf(&console_buf, "%d", &i);
			console_player_info(i);
			break;

		/* Wants to see the artifact list */
		case CONSOLE_ARTIFACT_LIST:
			console_artifacts();
			break;

		/* Wants to see the unique list */
		case CONSOLE_UNIQUE_LIST:
			console_uniques();
			break;

		/* Wants to change artifact status */
		case CONSOLE_CHANGE_ARTIFACT:
			Packet_scanf(&console_buf, "%d%d", &i, &j);
			console_change_artifact(i, j);
			break;

		/* Wants to change unique status */
		case CONSOLE_CHANGE_UNIQUE:
			Packet_scanf(&console_buf, "%d%s", &i, buf);
			console_change_unique(i, buf);
			break;

		/* Wants to send a message */
		case CONSOLE_MESSAGE:
			Packet_scanf(&console_buf, "%s", buf);
			console_message(buf);
			break;

		/* Wants to kick a player */
		case CONSOLE_KICK_PLAYER:
			Packet_scanf(&console_buf, "%s", buf);
			console_kick_player(buf);
			break;

		/* Wants to reload the server preferences */
		case CONSOLE_RELOAD_SERVER_PREFERENCES:
			console_reload_server_preferences();
			break;

		/* Wants to shut the server down */
		case CONSOLE_SHUTDOWN:
			console_shutdown();
			break;

		/* Strangeness */
		default:
			s_printf("Bizarre input on server console; ignoring.\n");
			return;
	}

	/* Write the response */
	DgramWrite(console_buf.sock, console_buf.ptr, console_buf.len);
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

#endif
