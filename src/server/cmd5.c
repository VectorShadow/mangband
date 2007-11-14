/* File: cmd5.c */

/* Purpose: Spell/Prayer commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#define SERVER

#include "angband.h"



/*
 * XXX XXX XXX XXX
 */
extern void display_spell_list(void);

/*
 * Hack -- Display all known spells in a window
 *
 * XXX XXX XXX Need to analyze size of the window.
 *
 * XXX XXX XXX Need more color coding.
 *
 * I dunno about this -- I've never found it very useful, so it'll be
 * gone from MAngband (for a while, at least).  I'll eventually just transmit
 * all of this information to client and let it handle it if the user wants
 * to have this in a window --KLJ--
 */
void display_spell_list(void)
{
}



/*
 * Returns spell chance of failure for spell		-RAK-
 */
static s16b spell_chance(int Ind, int spell)
{
	player_type *p_ptr = Players[Ind];

	int		chance, minfail;

	magic_type	*s_ptr;


	/* Paranoia -- must be literate */
	if (!p_ptr->mp_ptr->spell_book) return (100);

	/* Access the spell */
	s_ptr = &p_ptr->mp_ptr->info[spell];

	/* Extract the base spell failure rate */
	chance = s_ptr->sfail;

	/* Reduce failure rate by "effective" level adjustment */
	chance -= 3 * (p_ptr->lev - s_ptr->slevel);

	/* Reduce failure rate by INT/WIS adjustment */
    //chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[p_ptr->mp_ptr->spell_stat]] - 1);
    chance -= adj_mag_stat[p_ptr->stat_ind[p_ptr->mp_ptr->spell_stat]];

	/* Not enough mana to cast */
	if (s_ptr->smana > p_ptr->csp)
	{
		/* Hack -- Since at the moment casting spells without enough mana*/
		/* is impossible, I'm taking this out, as it confuses people. */
		/* chance += 5 * (s_ptr->smana - p_ptr->csp); */
	}

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[p_ptr->stat_ind[p_ptr->mp_ptr->spell_stat]];

    /* Non mage/priest/sorceror characters never get too good */
    if ((p_ptr->pclass != CLASS_MAGE) && (p_ptr->pclass != CLASS_PRIEST) 
#if defined(NEW_ADDITIONS)
		    && (p_ptr->pclass != CLASS_SORCEROR)
#endif
		    )
	{
		if (minfail < 5) minfail = 5;
	}

	/* Hack -- Priest prayer penalty for "edged" weapons  -DGK */
    if ((p_ptr->pclass == CLASS_PRIEST) && (p_ptr->icky_wield)) chance += 25;

	/* Minimum failure rate */
	if (chance < minfail) chance = minfail;

	/* Stunning makes spells harder */
	if (p_ptr->stun > 50) chance += 25;
	else if (p_ptr->stun) chance += 15;

	/* Always a 5 percent chance of working */
	if (chance > 95) chance = 95;

	/* Return the chance */
	return (chance);
}



/*
 * Determine if a spell is "okay" for the player to cast or study
 * The spell must be legible, not forgotten, and also, to cast,
 * it must be known, and to study, it must not be known.
 */
static bool spell_okay(int Ind, int j, bool known)
{
	player_type *p_ptr = Players[Ind];

	magic_type *s_ptr;

	/* Access the spell */
	s_ptr = &p_ptr->mp_ptr->info[j];

	/* Spell is illegal */
	if (s_ptr->slevel > p_ptr->lev) return (FALSE);

	/* Spell is forgotten */
	if ((j < 32) ?
	    (p_ptr->spell_forgotten1 & (1L << j)) :
	    (p_ptr->spell_forgotten2 & (1L << (j - 32))))
	{
		/* Never okay */
		return (FALSE);
	}

	/* Spell is learned */
	if ((j < 32) ?
	    (p_ptr->spell_learned1 & (1L << j)) :
	    (p_ptr->spell_learned2 & (1L << (j - 32))))
	{
		/* Okay to cast, not to study */
		return (known);
	}

	/* Okay to study, not to cast */
	return (!known);
}



/*
 * Extra information on a spell		-DRS-
 *
 * We can use up to 14 characters of the buffer 'p'
 *
 * The strings in this function were extracted from the code in the
 * functions "do_cmd_cast()" and "do_cmd_pray()" and may be dated.
 */
static void spell_info(int Ind, char *p, int j)
{
	player_type *p_ptr = Players[Ind];

	/* Default */
	strcpy(p, "");

#ifdef DRS_SHOW_SPELL_INFO

	/* Mage spells */
	if (p_ptr->mp_ptr->spell_book == TV_MAGIC_BOOK)
	{
		int plev = p_ptr->lev;

		/* Analyze the spell */
		switch (j)
		{
            case MSPELL_MAGIC_MISSILE: sprintf(p, " dam %dd4", 3+((plev-1)/5)); break;
            case MSPELL_PHASE_DOOR: strcpy(p, " range 10"); break;
            case MSPELL_CURE_LIGHT: strcpy(p, " heal 2d8"); break;
            case MSPELL_STINKING_CLOUD: sprintf(p, " dam %d", 10 + (plev / 2)); break;
            case MSPELL_LIGHTNING_BOLT: sprintf(p, " dam %dd8", (3+((plev-5)/4))); break;
            case MSPELL_TELEPORT_SELF: sprintf(p, " range %d", plev * 5); break;
            case MSPELL_SPEAR_LIGHT: strcpy(p, " dam 6d8"); break;
            case MSPELL_FROST_BOLT: sprintf(p, " dam %dd8", (5+((plev-5)/4))); break;
            case MSPELL_FIRE_BOLT: sprintf(p, " dam %dd8", (8+((plev-5)/4))); break;
            case MSPELL_FROST_BALL: sprintf(p, " dam %d", 30 + plev); break;
	    case MSPELL_FIRE_BALL: sprintf(p, " dam %d", 55 + plev); break;
            case MSPELL_HASTE_SELF: sprintf(p, " dur %d+d20", plev); break;
	    case MSPELL_TIDAL_WAVE: sprintf(p, " dam %d", 40 + plev*2); break;            
            case MSPELL_ACID_BOLT: sprintf(p, " dam %dd8", (6+((plev-5)/4))); break;
            case MSPELL_CLOUD_KILL: sprintf(p, " dam %d", 20 + plev/2); break;
            case MSPELL_ACID_BALL: sprintf(p, " dam %d", 40 + plev); break;
            case MSPELL_ICE_STORM: sprintf(p, " dam %d", 70 + plev); break;
            case MSPELL_METEOR_SWARM: sprintf(p, " dam %d", 65 + plev); break;
            case MSPELL_MANA_STORM: sprintf(p, " dam %d", 300 + plev*2); break;
            case MSPELL_RESIST_FIRE: strcpy(p, " dur 20+d20"); break;
            case MSPELL_RESIST_COLD: strcpy(p, " dur 20+d20"); break;
            case MSPELL_RESIST_ACID: strcpy(p, " dur 20+d20"); break;
            case MSPELL_RESIST_POISON: strcpy(p, " dur 20+d20"); break;
            case MSPELL_RESIST_ELEMENTS: strcpy(p, " dur 20+d20"); break;
            case MSPELL_SHIELD_HERO: strcpy(p, " dur 25+d25"); break;
            case MSPELL_SHIELD_ARMOR: strcpy(p, " dur 30+d20"); break;
            case MSPELL_SHIELD_BERSERK: strcpy(p, " dur 25+d25"); break;
            case MSPELL_SHIELD_SPEED: sprintf(p, " dur %d+d30", 30+plev); break;
            case MSPELL_SHIELD_GOI: strcpy(p, " dur 10+d10"); break;
		}
	}

	/* Priest spells */
	if (p_ptr->mp_ptr->spell_book == TV_PRAYER_BOOK)
	{
		int plev = p_ptr->lev;

		/* See below */
        int orb = (plev / ((p_ptr->pclass == CLASS_PRIEST) ? 2 : 4));

        /* Analyze the spell */
        switch (j)
        {
            case PSPELL_CURE_LIGHT: strcpy(p, " heal 2d10"); break;
            case PSPELL_HERO_BLESS: strcpy(p, " dur 12+d12"); break;
            case PSPELL_TELEPORT_PORTAL: sprintf(p, " range %d", 3*plev); break;
            case PSPELL_CURE_SERIOUS: strcpy(p, " heal 4d10"); break;
            case PSPELL_HERO_CHANT: strcpy(p, " dur 24+d24"); break;
            case PSPELL_RESIST_LOWS: strcpy(p, " dur 10+d10"); break;
            case PSPELL_ORB_DRAINING: sprintf(p, " %d+3d6", plev + orb); break;
            case PSPELL_CURE_CRITICAL: strcpy(p, " heal 6d10"); break;
            case PSPELL_SENSE_INVISIBLE: strcpy(p, " dur 24+d24"); break;
            case PSPELL_PROTECTION_EVIL: sprintf(p, " dur %d+d25", 3*plev); break;
            case PSPELL_CURE_MORTAL: strcpy(p, " heal 10d10"); break;
            case PSPELL_HERO_PRAYER: strcpy(p, " dur 48+d48"); break;
            case PSPELL_DISPEL_UNDEAD: sprintf(p, " dam d%d", 3*plev); break;
            case PSPELL_CURE_HEAL: strcpy(p, " heal 300"); break;
            case PSPELL_DISPEL_EVIL: sprintf(p, " dam d%d", 3*plev); break;
            case PSPELL_HOLY_WORD: strcpy(p, " heal 1000"); break;
            case PSPELL_CURE_CRIT2: strcpy(p, " heal 6d10"); break;
            case PSPELL_CURE_HEALING: strcpy(p, " heal 100"); break;
            case PSPELL_EXTRA_HEALING: strcpy(p, " heal 2000"); break;
            case PSPELL_DISPEL_UNDEAD2: sprintf(p, " dam d%d", 4*plev); break;
            case PSPELL_DISPEL_EVIL2: sprintf(p, " dam d%d", 4*plev); break;
            case PSPELL_ANNIHILAT_BOLT: strcpy(p, " dam 200"); break;
            case PSPELL_TELEPORT_BLINK: strcpy(p, " range 10"); break;
            case PSPELL_TELEPORT_SELF: sprintf(p, " range %d", 8*plev); break;
        }
    }
#if defined(NEW_ADDITIONS)
    /* Sorcery spells */
    if (p_ptr->mp_ptr->spell_book == TV_SORCERY_BOOK)
    {
	int plev = p_ptr->lev;

		/* Analyze the spell */
		switch (j)
		{
		case SSPELL_MAGIC_MISSILE: sprintf(p, " dam %dd%d", 3+((plev-1)/5), 4 + (plev / 10)); break;
                case SSPELL_PHASE_DOOR: sprintf(p, " range %d", 10); break;
		case SSPELL_CONFUSE_MONSTER: sprintf(p, " power %d", (plev * 3) / 2); break;
                case SSPELL_NOXIOUS_CLOUD: sprintf(p, " dam %d", 10 + (plev / 2)); break;
                case SSPELL_TELEPORT_SELF: sprintf(p, " range %d", plev * 5); break;
                case SSPELL_LIGHTNING_BOLT: sprintf(p, " dam %dd%d", 5 + ((plev - 5) / 4), 8); break;
		case SSPELL_FROST_BOLT: sprintf(p, " dam %dd%d", 6+((plev-5)/4),8); break;
                case SSPELL_WRAITH_FORM: sprintf(p, " dur %d+d20", 10 + plev); break;
		case SSPELL_ETHEREAL_EYE: sprintf(p, " dur %d+d20", 20); break;
                case SSPELL_FIRE_BOLT: sprintf(p, " dam %dd%d", 9+((plev-5)/4), 8); break;
                case SSPELL_HASTE_SELF: sprintf(p, " dur %d+d%d", plev, 20 + plev); break;
                case SSPELL_ELEMENTAL_BOLT: sprintf(p, " dam 4*%dd%d", 5+((plev-5)/4), 8); break;
                case SSPELL_MEDITATE_MANA: sprintf(p, " dur %d+d20", 20); break;
                case SSPELL_FIRE_STORM: sprintf(p, " dam %d", 100 + (3 * plev)); break;
                case SSPELL_FORCE_SHIELD: sprintf(p, " dur %d+d20", 30); break;
                case SSPELL_SUN_FIRE: sprintf(p, " dam %d", 130 + (plev * 5 / 2)); break;
                case SSPELL_ELEM_SHIELD: sprintf(p, " dur %d+d20", 20); break;
		case SSPELL_MANA_SHIELD: sprintf(p, " dur 8+d10"); break;
		case SSPELL_CHAOS_BLAST: sprintf(p, " dam %d", 100 + (3 * plev)); break;
		case SSPELL_MANA_BLAST: sprintf(p, " dam %d", 600 + (2 * plev)); break;
                case SSPELL_PLASMA_BLAST: sprintf(p, " dam %d", 400 + (2 * plev)); break;
                case SSPELL_ANNIHIL_BOLT: sprintf(p, " dam %d", 500 + (2 * plev)); break;
                case SSPELL_TIDAL_WAVE: sprintf(p, " dam %d", 450 + (2 * plev)); break;
                case SSPELL_ANARCHY_FORCE: sprintf(p, " dam 5*%d", 250 + plev); break;
                case SSPELL_MANA_STRIKE: sprintf(p, " dam %d", 800); break;
		}
	}
# endif 

#endif

}


/*
 * Print a list of spells (for browsing or casting)
 */
static void print_spells(int Ind, int book, byte *spell, int num)
{
	player_type *p_ptr = Players[Ind];

	int			i, j;

	magic_type		*s_ptr;

	cptr		comment;

	char		info[80];

	char		out_val[160];


	/* Dump the spells */
	for (i = 0; i < num; i++)
	{
		/* Access the spell */
		j = spell[i];

		/* Access the spell */
		s_ptr = &p_ptr->mp_ptr->info[j];

		/* Skip illegible spells */
		if (s_ptr->slevel >= 99)
		{
			sprintf(out_val, "  %c) %-30s", I2A(i), "(illegible)");
			Send_spell_info(Ind, book, i, out_val);
			continue;
		}

		/* XXX XXX Could label spells above the players level */

		/* Get extra info */
		spell_info(Ind, info, j);

		/* Use that info */
		comment = info;

		/* Analyze the spell */
		if ((j < 32) ?
		    ((p_ptr->spell_forgotten1 & (1L << j))) :
		    ((p_ptr->spell_forgotten2 & (1L << (j - 32)))))
		{
			comment = " forgotten";
		}
		else if (!((j < 32) ?
		           (p_ptr->spell_learned1 & (1L << j)) :
		           (p_ptr->spell_learned2 & (1L << (j - 32)))))
		{
			comment = " unknown";
		}
		else if (!((j < 32) ?
		           (p_ptr->spell_worked1 & (1L << j)) :
		           (p_ptr->spell_worked2 & (1L << (j - 32)))))
		{
			comment = " untried";
		}

		/* Dump the spell --(-- */
		sprintf(out_val, "  %c) %-30s%2d %4d %3d%%%s",
		        I2A(i), spell_names[p_ptr->mp_ptr->spell_type][j],
		        s_ptr->slevel, s_ptr->smana, spell_chance(Ind, j), comment);
		Send_spell_info(Ind, book, i, out_val);
	}
}



/*
 * Allow user to choose a spell/prayer from the given book.
 *
 * If a valid spell is chosen, saves it in '*sn' and returns TRUE
 * If the user hits escape, returns FALSE, and set '*sn' to -1
 * If there are no legal choices, returns FALSE, and sets '*sn' to -2
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 */
#if 0
static int get_spell(int Ind, int *sn, cptr prompt, int sval, bool known)
{
}
#endif




/*
 * Peruse the spells/prayers in a Book
 *
 * Note that *all* spells in the book are listed
 */
void do_cmd_browse(int Ind, int book)
{
	player_type *p_ptr = Players[Ind];

	int			i, item, sval;

	byte		spell[64], num = 0;

	object_type		*o_ptr;


	/* Warriors are illiterate */
	if (!p_ptr->mp_ptr->spell_book)
	{
		msg_print(Ind, "You cannot read books!");
		return;
	}

	/* No lite */
	if (p_ptr->blind || no_lite(Ind))
	{
		msg_print(Ind, "You cannot see!");
		return;
	}

	/* Confused */
	if (p_ptr->confused)
	{
		msg_print(Ind, "You are too confused!");
		return;
	}


	/* Restrict choices to "useful" books */
	item_tester_tval = p_ptr->mp_ptr->spell_book;

	item = book;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	if (o_ptr->tval != p_ptr->mp_ptr->spell_book)
	{
		msg_print(Ind, "SERVER ERROR: Tried browsing a bad book!");
		return;
	}


	/* Access the item's sval */
	sval = o_ptr->sval;


	/* Extract spells */
	for (i = 0; i < 64; i++)
	{
		/* Check for this spell */
		if ((i < 32) ?
		    (spell_flags[p_ptr->mp_ptr->spell_type][sval][0] & (1L << i)) :
		    (spell_flags[p_ptr->mp_ptr->spell_type][sval][1] & (1L << (i - 32))))
		{
			/* Collect this spell */
			spell[num++] = i;
		}
	}


	/* Display the spells */
	print_spells(Ind, book, spell, num);
}




/*
 * Study a book to gain a new spell/prayer
 */
void do_cmd_study(int Ind, int book, int spell)
{
	player_type *p_ptr = Players[Ind];

	int			i, sval;

	int			j = -1;

    cptr p = ((p_ptr->mp_ptr->spell_book == TV_PRAYER_BOOK) ? "prayer" : "spell");

	object_type		*o_ptr;

	byte spells[64], num = 0;


	if (!p_ptr->mp_ptr->spell_book)
	{
		msg_print(Ind, "You cannot read books!");
		return;
	}

	if (p_ptr->blind || no_lite(Ind))
	{
		msg_print(Ind, "You cannot see!");
		return;
	}

	if (p_ptr->confused)
	{
		msg_print(Ind, "You are too confused!");
		return;
	}

	if (!(p_ptr->new_spells))
	{
		msg_format(Ind, "You cannot learn any new %ss!", p);
		return;
	}


	/* Restrict choices to "useful" books */
	item_tester_tval = p_ptr->mp_ptr->spell_book;

	/* Get the item (in the pack) */
	if (book >= 0)
	{
		o_ptr = &p_ptr->inventory[book];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - book];
	}

	if (o_ptr->tval != p_ptr->mp_ptr->spell_book)
	{
		msg_print(Ind, "SERVER ERROR: Trying to gain a spell from a bad book!");
		return;
	}

	/* Access the item's sval */
	sval = o_ptr->sval;


    /* Spellcaster -- Learn a selected spell */
    if ((p_ptr->mp_ptr->spell_book == TV_MAGIC_BOOK) 
#if defined(NEW_ADDITIONS)
		    || (p_ptr->mp_ptr->spell_book == TV_SORCERY_BOOK)
#endif
		    )
	{
		for (i = 0; i < 64; i++)
		{
			/* Check for this spell */
			if ((i < 32) ?
				(spell_flags[p_ptr->mp_ptr->spell_type][sval][0] & (1L << i)) :
				(spell_flags[p_ptr->mp_ptr->spell_type][sval][1] & (1L << (i - 32))))
			{
				/* Collect this spell */
				spells[num++] = i;
			}
		}

		/* Set the spell number */
		j = spells[spell];

		if (!spell_okay(Ind, j, FALSE))
		{
			msg_print(Ind, "You cannot gain that spell!");
			return;
		}
	}

    /* Cleric -- Learn a random prayer */
	if (p_ptr->mp_ptr->spell_book == TV_PRAYER_BOOK)
	{
		int k = 0;

		/* Extract spells */
		for (i = 0; i < 64; i++)
		{
			/* Check spells in the book */
			if ((i < 32) ?
			    (spell_flags[p_ptr->mp_ptr->spell_type][sval][0] & (1L << i)) :
			    (spell_flags[p_ptr->mp_ptr->spell_type][sval][1] & (1L << (i - 32))))
			{
				/* Skip non "okay" prayers */
				if (!spell_okay(Ind, i, FALSE)) continue;

				/* Hack -- Prepare the randomizer */
				k++;

				/* Hack -- Apply the randomizer */
				if (rand_int(k) == 0) j = i;
			}
		}
	}

	/* Nothing to study */
	if (j < 0)
	{
		/* Message */
		msg_format(Ind, "You cannot learn any %ss in that book.", p);

		/* Abort */
		return;
	}


	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Learn the spell */
	if (j < 32)
	{
		p_ptr->spell_learned1 |= (1L << j);
	}
	else
	{
		p_ptr->spell_learned2 |= (1L << (j - 32));
	}

	/* Find the next open entry in "spell_order[]" */
	for (i = 0; i < 64; i++)
	{
		/* Stop at the first empty space */
		if (p_ptr->spell_order[i] == 99) break;
	}

	/* Add the spell to the known list */
	p_ptr->spell_order[i++] = j;

	/* Mention the result */
	msg_format(Ind, "You have learned the %s of %s.",
	           p, spell_names[p_ptr->mp_ptr->spell_type][j]);

	/* One less spell available */
	p_ptr->new_spells--;

	/* Report on remaining prayers */
	if (p_ptr->new_spells)
	{
		msg_format(Ind, "You can learn %d more %ss.", p_ptr->new_spells, p);
	}

	/* Save the new_spells value */
	p_ptr->old_spells = p_ptr->new_spells;

	/* Redraw Study Status */
	p_ptr->redraw |= (PR_STUDY);

	/* Update the spell info */
	p_ptr->window |= (PW_SPELL);
}



/*
 * Cast a spell
 *
 * Many of the spells have a strange get_aim_dir(), and then they return.
 * What this does is it sends a PKT_GET_DIRECTION to the client, which then
 * (when the player hits a direction key), send a response to the server.  The
 * server then calls do_cmd_cast_aux() with the direction.  This is a crappy
 * way of doing things, but it should work.  Without this, the server is hung
 * until the player hits a direction key, and we try very hard not to have
 * any undue slowness in the server. --KLJ--
 */
void do_cmd_cast(int Ind, int book, int spell)
{
	player_type *p_ptr = Players[Ind];

	int			i, j, sval;
	int			chance, beam;
	int			plev = p_ptr->lev;

	object_type		*o_ptr;

	magic_type		*s_ptr;

	byte spells[64], num = 0;

	/* Require spell ability */
	if (p_ptr->mp_ptr->spell_book != TV_MAGIC_BOOK)
	{
		msg_print(Ind, "You cannot cast spells!");
		return;
	}

	/* Require lite */
	if (p_ptr->blind || no_lite(Ind))
	{
		msg_print(Ind, "You cannot see!");
		return;
	}

	/* Not when confused */
	if (p_ptr->confused)
	{
		msg_print(Ind, "You are too confused!");
		return;
	}


	/* Restrict choices to spell books */
	item_tester_tval = p_ptr->mp_ptr->spell_book;

	/* Get the item (in the pack) */
	if (book >= 0)
	{
		o_ptr = &p_ptr->inventory[book];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - book];
	}

	if (o_ptr->tval != p_ptr->mp_ptr->spell_book)
	{
		msg_print(Ind, "SERVER ERROR: Tried to cast spell from bad book!");
		return;
	}

        if( check_guard_inscription( o_ptr->note, 'm' )) {
                msg_print(Ind, "The item's inscription prevents it");
                return;
        };

	/* Access the item's sval */
	sval = o_ptr->sval;

	for (i = 0; i < 64; i++)
	{
		/* Check for this spell */
		if ((i < 32) ?
			(spell_flags[p_ptr->mp_ptr->spell_type][sval][0] & (1L << i)) :
			(spell_flags[p_ptr->mp_ptr->spell_type][sval][1] & (1L << (i - 32))))
		{
			/* Collect this spell */
			spells[num++] = i;
		}
	}

	/* Set the spell number */
	j = spells[spell];

	if (!spell_okay(Ind, j, 1))
	{
		msg_print(Ind, "You cannot cast that spell.");
		return;
	}

	/* Access the spell */
	s_ptr = &p_ptr->mp_ptr->info[j];

	/* Check mana */
	if (s_ptr->smana > p_ptr->csp)
	{
		msg_print(Ind, "You do not have enough mana.");
		return;
	}

	/* Spell failure chance */
	chance = spell_chance(Ind, j);

	/* Failed spell */
	if (rand_int(100) < chance)
	{
		if (flush_failure) flush();
		msg_print(Ind, "You failed to get the spell off!");
	}

	/* Process spell */
	else
	{
		/* Hack -- chance of "beam" instead of "bolt" */
        beam = ((p_ptr->pclass == CLASS_MAGE) ? plev : (plev / 2));

		/* Spells.  */
		switch (j)
		{
            case MSPELL_MAGIC_MISSILE:
			{
                p_ptr->current_spell = MSPELL_MAGIC_MISSILE;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_DETECT_MONSTERS:
			{
				(void)detect_creatures(Ind);
				break;
			}

            case MSPELL_PHASE_DOOR:
			{
				msg_format_near(Ind, "%s blinks away!", p_ptr->name);
				teleport_player(Ind, 10);
				break;
			}

            case MSPELL_LIGHT_AREA:
			{
				(void)lite_area(Ind, damroll(2, (plev / 2)), (plev / 10) + 1);
				break;
			}

            case MSPELL_DETECT_TREASURES:
			{
				(void)detect_treasure(Ind);
				break;
			}

            case MSPELL_CURE_LIGHT:
			{
				(void)hp_player(Ind, damroll(2, 8));
				(void)set_cut(Ind, p_ptr->cut - 15);
				break;
			}

            case MSPELL_DETECT_OBJECTS:
			{
				(void)detect_object(Ind);
				break;
			}

            case MSPELL_DETECT_FEATURES:
			{
				(void)detect_sdoor(Ind);
				(void)detect_trap(Ind);
				break;
			}

            case MSPELL_STINKING_CLOUD:
			{
                p_ptr->current_spell = MSPELL_STINKING_CLOUD;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_CONFUSE_MONSTER:
			{
                p_ptr->current_spell = MSPELL_CONFUSE_MONSTER;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_LIGHTNING_BOLT:
			{
                p_ptr->current_spell = MSPELL_LIGHTNING_BOLT;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_DESTROY_FEATURES:
			{
				(void)destroy_doors_touch(Ind);
				break;
			}

            case MSPELL_SLEEP_MONSTER:
			{
                p_ptr->current_spell = MSPELL_SLEEP_MONSTER;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_CURE_POISON:
			{
				(void)set_poisoned(Ind, 0);
				break;
			}

            case MSPELL_TELEPORT_SELF:
			{
				msg_format_near(Ind, "%s teleports away!", p_ptr->name);
				teleport_player(Ind, plev * 5);
				break;
			}

            case MSPELL_SPEAR_LIGHT:
			{
                p_ptr->current_spell = MSPELL_SPEAR_LIGHT;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_FROST_BOLT:
			{
                p_ptr->current_spell = MSPELL_FROST_BOLT;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_STONE_MUD:
			{
                p_ptr->current_spell = MSPELL_STONE_MUD;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_SAT_HUNGER:
			{
				(void)set_food(Ind, PY_FOOD_MAX - 1);
				break;
			}

            case MSPELL_RECHARGE_ITEM:
			{
				(void)recharge(Ind, 5);
				break;
			}

	    case MSPELL_FIRE_BOLT:
			{
                p_ptr->current_spell = MSPELL_FIRE_BOLT;
                get_aim_dir(Ind);
                return;
			}

            case MSPELL_POLYMORPH_OTHER:
			{
                p_ptr->current_spell = MSPELL_POLYMORPH_OTHER;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_IDENTIFY_OBJECT:
			{
				(void)ident_spell(Ind);
				break;
			}

            case MSPELL_SLEEP_MONSTERS:
			{
				(void)sleep_monsters(Ind);
				break;
			}

            case MSPELL_FROST_BALL:
			{
                p_ptr->current_spell = MSPELL_FROST_BALL;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_SLOW_MONSTER:
			{
                p_ptr->current_spell = MSPELL_SLOW_MONSTER;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_FIRE_BALL:
			{
                p_ptr->current_spell = MSPELL_FIRE_BALL;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_RECHARGE_ITEM2:
			{
				(void)recharge(Ind, 40);
				break;
			}

            case MSPELL_TELEPORT_OTHER:
			{
                p_ptr->current_spell = MSPELL_TELEPORT_OTHER;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_HASTE_SELF:
			{
				msg_format_near(Ind, "%s starts moving faster.", p_ptr->name);
				if (!p_ptr->fast)
				{
					(void)set_fast(Ind, randint(20) + plev);
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + randint(5));
				}
				break;
			}

            case MSPELL_TIDAL_WAVE:
			{
                p_ptr->current_spell = MSPELL_TIDAL_WAVE;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_WORD_DESTRUCTION:
			{
				msg_format_near(Ind, "%s unleashes great power!", p_ptr->name);
				destroy_area(p_ptr->dun_depth, p_ptr->py, p_ptr->px, 15, TRUE);
				break;
			}

            case MSPELL_GENOCIDE_HARD:
			{
				(void)genocide(Ind);
				break;
			}

            case MSPELL_DOOR_CREATION:
			{
				(void)door_creation(Ind);
				break;
			}

            case MSPELL_STAIR_CREATION:
			{
				(void)stair_creation(Ind);
				break;
			}

            case MSPELL_TELEPORT_LEVEL:
			{
				(void)teleport_player_level(Ind);
				break;
			}

            case MSPELL_EARTH_QUAKE:
			{
				msg_format_near(Ind, "%s casts a spell, and the ground shakes!", p_ptr->name);
				earthquake(p_ptr->dun_depth, p_ptr->py, p_ptr->px, 10);
				break;
			}

            case MSPELL_WORD_RECALL:
			{
		if (cfg_ironman)
				msg_print(Ind, "The air about you becomes charged... but only for a moment...");
		else if (!p_ptr->word_recall)
				{
					set_recall_depth(p_ptr, o_ptr);
					p_ptr->word_recall = rand_int(20) + 15;
					msg_print(Ind, "The air about you becomes charged...");
				}
				else
				{
					p_ptr->word_recall = 0;
					msg_print(Ind, "A tension leaves the air around you...");
				}
				break;
			}

            case MSPELL_ACID_BOLT:
			{
                p_ptr->current_spell = MSPELL_ACID_BOLT;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_CLOUD_KILL:
			{
                p_ptr->current_spell = MSPELL_CLOUD_KILL;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_ACID_BALL:
			{
                p_ptr->current_spell = MSPELL_ACID_BALL;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_ICE_STORM:
			{
                p_ptr->current_spell = MSPELL_ICE_STORM;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_METEOR_SWARM:
			{
                p_ptr->current_spell = MSPELL_METEOR_SWARM;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_MANA_STORM:
			{
                p_ptr->current_spell = MSPELL_MANA_STORM;
				get_aim_dir(Ind);
				return;
			}

            case MSPELL_DETECT_EVIL:
			{
				(void)detect_evil(Ind);
				break;
			}

            case MSPELL_DETECT_ENCHANT:
			{
				(void)detect_magic(Ind);
				break;
			}

            case MSPELL_RECHARGE_ITEM3:
			{
				recharge(Ind, 100);
				break;
			}

            case MSPELL_GENOCIDE_EASY:
			{
				(void)genocide(Ind);
				break;
			}

            case MSPELL_GENOCIDE_MASS:
			{
				(void)mass_genocide(Ind);
				break;
			}

            case MSPELL_RESIST_FIRE:
			{
				(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(20) + 20);
				break;
			}

            case MSPELL_RESIST_COLD:
			{
				(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(20) + 20);
				break;
			}

            case MSPELL_RESIST_ACID:
			{
				(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(20) + 20);
				break;
			}

            case MSPELL_RESIST_POISON:
			{
				(void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(20) + 20);
				break;
			}

            case MSPELL_RESIST_ELEMENTS:
			{
				(void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(20) + 20);
				(void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(20) + 20);
				(void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(20) + 20);
				(void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(20) + 20);
				(void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(20) + 20);
				break;
			}

            case MSPELL_SHIELD_HERO:
			{
				(void)hp_player(Ind, 10);
				(void)set_hero(Ind, p_ptr->hero + randint(25) + 25);
				(void)set_afraid(Ind, 0);
				break;
			}

            case MSPELL_SHIELD_ARMOR:
			{
				msg_format_near(Ind, "%s forms a mystic shield.", p_ptr->name);
				(void)set_shield(Ind, p_ptr->shield + randint(20) + 30);
				break;
			}

            case MSPELL_SHIELD_BERSERK:
			{
				msg_format_near(Ind, "%s enters a battle rage!", p_ptr->name);
				(void)hp_player(Ind, 30);
				(void)set_shero(Ind, p_ptr->shero + randint(25) + 25);
				(void)set_afraid(Ind, 0);
				break;
			}

            case MSPELL_SHIELD_SPEED:
			{
				if (!p_ptr->fast)
				{
					(void)set_fast(Ind, randint(30) + 30 + plev);
				}
				else
				{
					(void)set_fast(Ind, p_ptr->fast + randint(10));
				}
				break;
			}

            case MSPELL_SHIELD_GOI:
			{
				//(void)set_invuln(Ind, p_ptr->invuln + randint(8) + 8);
				// GOI no longer stacks, as when it did it was vastly overpowered. 
				if (!p_ptr->invuln)
					set_invuln(Ind, 10 + randint(10));
				break;
			}
		}

		/* A spell was cast */
		if (!((j < 32) ?
		      (p_ptr->spell_worked1 & (1L << j)) :
		      (p_ptr->spell_worked2 & (1L << (j - 32)))))
		{
			int e = s_ptr->sexp;

			/* The spell worked */
			if (j < 32)
			{
				p_ptr->spell_worked1 |= (1L << j);
			}
			else
			{
				p_ptr->spell_worked2 |= (1L << (j - 32));
			}

			/* Gain experience */
			gain_exp(Ind, e * s_ptr->slevel);

			/* Fix the spell info */
			p_ptr->window |= PW_SPELL;
		}
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Sufficient mana */
	if (s_ptr->smana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= s_ptr->smana;
	}

	/* Over-exert the player */
	else
	{
		int oops = s_ptr->smana - p_ptr->csp;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
		msg_print(Ind, "You faint from the effort!");

		/* Hack -- Bypass free action */
		(void)set_paralyzed(Ind, p_ptr->paralyzed + randint(5 * oops + 1));

		/* Damage CON (possibly permanently) */
		if (rand_int(100) < 50)
		{
			bool perm = (rand_int(100) < 25);

			/* Message */
			msg_print(Ind, "You have damaged your health!");

			/* Reduce constitution */
			(void)dec_stat(Ind, A_CON, 15 + randint(10), perm);
		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
}


/*
 * Finish casting a spell that required a direction --KLJ--
 */
void do_cmd_cast_aux(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];

	int plev = p_ptr->lev;
    int beam = ((p_ptr->pclass == CLASS_MAGE) ? plev : (plev / 2));

	magic_type *s_ptr = &p_ptr->mp_ptr->info[p_ptr->current_spell];

	/* Only fire in direction 5 if we have a target */
	if ((dir == 5) && !target_okay(Ind))
	{
		/* Reset current spell */
		p_ptr->current_spell = -1;

		/* Done */
		return;
	}

	/* We assume that the spell can be cast, and so forth */
	switch(p_ptr->current_spell)
	{
        case MSPELL_MAGIC_MISSILE:
		{
			msg_format_near(Ind, "%s fires a magic missile.", p_ptr->name);
			fire_bolt_or_beam(Ind, beam-10, GF_MISSILE, dir,
						damroll(3 + ((plev - 1) / 5), 4));
			break;
		}

        case MSPELL_STINKING_CLOUD:
		{
			msg_format_near(Ind, "%s casts a stinking cloud.", p_ptr->name);
			fire_ball(Ind, GF_POIS, dir,
				10 + (plev / 2), 2);
			break;
		}

        case MSPELL_CONFUSE_MONSTER:
		{
			confuse_monster(Ind, dir, plev);
			break;
		}

        case MSPELL_LIGHTNING_BOLT:
		{
			msg_format_near(Ind, "%s casts a lightning bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, beam-10, GF_ELEC, dir,
				damroll(3+((plev-5)/4), 8));
			break;
		}

        case MSPELL_SLEEP_MONSTER:
		{
			sleep_monster(Ind, dir);
			break;
		}

        case MSPELL_SPEAR_LIGHT:
		{
			msg_print(Ind, "A line of blue shimmering light appears.");
			lite_line(Ind, dir);
			break;
		}

        case MSPELL_FROST_BOLT:
		{
			msg_format_near(Ind, "%s casts a frost bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, beam-10, GF_COLD, dir,
				damroll(5+((plev-5)/4), 8));
			break;
		}

        case MSPELL_STONE_MUD:
		{
			(void)wall_to_mud(Ind, dir);
			break;
		}

        case MSPELL_POLYMORPH_OTHER:
		{
			(void)poly_monster(Ind, dir);
			break;
		}

        case MSPELL_FIRE_BOLT:
		{
			msg_format_near(Ind, "%s casts a fire bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, beam, GF_FIRE, dir,
				damroll(8+((plev-5)/4), 8));
			break;
		}

        case MSPELL_SLOW_MONSTER:
		{
			(void)slow_monster(Ind, dir);
			break;
		}

        case MSPELL_FROST_BALL:
		{
			msg_format_near(Ind, "%s casts a frost ball.", p_ptr->name);
			fire_ball(Ind, GF_COLD, dir, 30 + (plev), 2);
			break;
		}

        case MSPELL_TELEPORT_OTHER:
		{
			(void)teleport_monster(Ind, dir);
			break;
		}

        case MSPELL_FIRE_BALL:
		{
			msg_format_near(Ind, "%s casts a fire ball.", p_ptr->name);
			fire_ball(Ind, GF_FIRE, dir, 55 + (plev), 2);
			break;
		}

	case MSPELL_TIDAL_WAVE:
        {
            msg_format_near(Ind, "%s invokes a tidal wave.", p_ptr->name);
            fire_ball(Ind, GF_WATER, dir, 40 + (plev * 2), 2);
            break;
        }

        case MSPELL_ACID_BOLT:
		{
			msg_format_near(Ind, "%s casts an acid bolt.", p_ptr->name);
			fire_bolt_or_beam(Ind, beam, GF_ACID, dir,
				damroll(6+((plev-5)/4), 8));
			break;
		}

        case MSPELL_CLOUD_KILL:
		{
			msg_format_near(Ind, "%s casts a cloud of death.", p_ptr->name);
			fire_ball(Ind, GF_POIS, dir, 20 + (plev / 2), 3);
			break;
		}

        case MSPELL_ACID_BALL:
		{
			msg_format_near(Ind, "%s casts an acid ball.", p_ptr->name);
			fire_ball(Ind, GF_ACID, dir, 40 + (plev), 2);
			break;
		}

        case MSPELL_ICE_STORM:
		{
            msg_format_near(Ind, "%s invokes an ice storm.", p_ptr->name);
			fire_ball(Ind, GF_COLD, dir, 70 + (plev), 3);
			break;
		}

        case MSPELL_METEOR_SWARM:
		{
			msg_format_near(Ind, "%s casts a meteor shower.", p_ptr->name);
			fire_ball(Ind, GF_METEOR, dir, 65 + (plev), 3);
			break;
		}

        case MSPELL_MANA_STORM:
		{
			msg_format_near(Ind, "%s casts a mana ball.", p_ptr->name);
			fire_ball(Ind, GF_MANA, dir, 300 + (plev * 2), 3);
			break;
		}

		default:  /* For some reason we got called for a spell that */
		{         /* doesn't require a direction */
			msg_print(Ind, "SERVER ERROR: do_cmd_cast_aux() called for non-directional spell!");
			p_ptr->current_spell = -1;
			return;
		}
	}	

	if (!((p_ptr->current_spell < 32) ?
		(p_ptr->spell_worked1 & (1L << p_ptr->current_spell)) :
		(p_ptr->spell_worked2 & (1L << (p_ptr->current_spell - 32)))))
	{
		int e = s_ptr->sexp;

		if (p_ptr->current_spell < 32)
		{
			p_ptr->spell_worked1 |= (1L << p_ptr->current_spell);
		}
		else
		{
			p_ptr->spell_worked2 |= (1L << (p_ptr->current_spell - 32));
		}

		gain_exp(Ind, e * s_ptr->slevel);

		/* Fix the spell info */
		p_ptr->window |= PW_SPELL;
	}

	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	if (s_ptr->smana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= s_ptr->smana;
	}

	/* Over-exert the player */
	else
	{
		int oops = s_ptr->smana - p_ptr->csp;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
		msg_print(Ind, "You faint from the effort!");

		/* Hack -- bypass free action */
		(void)set_paralyzed(Ind, p_ptr->paralyzed + randint(5 * oops + 1));

		/* Damage CON (possibly permanently) */
		if (rand_int(100) < 50)
		{
			bool perm = (rand_int(100) < 25);

			/* Message */
			msg_print(Ind, "You have damaged your health!");

			/* Reduce constitution */
			(void)dec_stat(Ind, A_CON, 15 + randint(10), perm);
		}
	}

	/* Reset current spell */
	p_ptr->current_spell = -1;

	/* Resend mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
}

#if defined(NEW_ADDITIONS)

/*
 * Cast a sorcery spell
 *
 * Many of the spells have a strange get_aim_dir(), and then they return.
 * What this does is it sends a PKT_GET_DIRECTION to the client, which then
 * (when the player hits a direction key), send a response to the server.  The
 * server then calls do_cmd_cast_aux() with the direction.  This is a crappy
 * way of doing things, but it should work.  Without this, the server is hung
 * until the player hits a direction key, and we try very hard not to have
 * any undue slowness in the server. --KLJ--
 */
void do_cmd_sorc(int Ind, int book, int spell)
{
	player_type *p_ptr = Players[Ind];

    int			i, j, sval;
    int			chance, beam;
	int plev = p_ptr->lev;

	object_type	*o_ptr;

	magic_type  *s_ptr;

	byte spells[64], num = 0;

    /* Require spell ability */
    if (p_ptr->mp_ptr->spell_book != TV_SORCERY_BOOK)
	{
        msg_print(Ind, "You cannot cast spells!");
		return;
	}

    /* Require lite */
	if (p_ptr->blind || no_lite(Ind))
	{
		msg_print(Ind, "You cannot see!");
		return;
	}

    /* Not when confused */
	if (p_ptr->confused)
	{
		msg_print(Ind, "You are too confused!");
		return;
	}


    /* Restrict choices to spell books */
	item_tester_tval = p_ptr->mp_ptr->spell_book;

	/* Get the item (in the pack) */
    if (book >= 0)
	{
        o_ptr = &p_ptr->inventory[book];
	}

	/* Get the item (on the floor) */
	else
	{
        o_ptr = &o_list[0 - book];
	}

	if (o_ptr->tval != p_ptr->mp_ptr->spell_book)
	{
        msg_print(Ind, "SERVER ERROR: Tried to cast spell from bad book!");
		return;
	}

        if( check_guard_inscription( o_ptr->note, 'm' )) {
                msg_print(Ind, "The item's inscription prevents it");
                return;
        };

	/* Access the item's sval */
	sval = o_ptr->sval;

	for (i = 0; i < 64; i++)
	{
		/* Check for this spell */
		if ((i < 32) ?
			(spell_flags[p_ptr->mp_ptr->spell_type][sval][0] & (1L << i)) :
			(spell_flags[p_ptr->mp_ptr->spell_type][sval][1] & (1L << (i - 32))))
		{
			/* Collect this spell */
			spells[num++] = i;
		}
	}
	
    /* Set the spell number */
    if (spell < 64)
	j = spells[spell];	
    else
	/* Affecting other spells */
	j = spells[spell-64];
	
	if (!spell_okay(Ind, j, 1))
	{
        msg_print(Ind, "You cannot cast that spell.");
		return;
	}
	
    /* Access the spell */
	s_ptr = &p_ptr->mp_ptr->info[j];
	
	/* Check mana */
	if (s_ptr->smana > p_ptr->csp)
	{
		msg_print(Ind, "You do not have enough mana.");
		return;
	}

	/* Spell failure chance */
	chance = spell_chance(Ind, j);

    /* Failed spell */
	if (rand_int(100) < chance)
	{
		if (flush_failure) flush();
        msg_print(Ind, "You failed to get the spell off!");
	}

    /* Process spell */
	else
	{
        /* Hack -- chance of "beam" instead of "bolt" */
        beam = ((p_ptr->pclass == CLASS_SORCEROR) ? plev : (plev / 2));

		if (spell >= 64) j += 64;

        /* Spells. */
		switch (j)
		{
            case SSPELL_MAGIC_MISSILE:
			{
                p_ptr->current_spell = SSPELL_MAGIC_MISSILE;
                get_aim_dir(Ind);
                return;
			}

	    case SSPELL_PHASE_DOOR:
			{
                msg_format_near(Ind, "%s blinks away!", p_ptr->name);
                teleport_player(Ind, 10);
				break;
			}

            case SSPELL_DETECT_MONSTERS:
			{
                (void)detect_creatures(Ind);
				break;
			}

	    case SSPELL_DETECT_TRAPS:
			{
                (void)detect_trap(Ind);
				break;
			}

            case SSPELL_LIGHT_AREA:
			{
				(void)lite_area(Ind, damroll(2, (plev / 2)), (plev / 10) + 1);
				break;
			}

            case SSPELL_DETECT_FEATURES:
			{
                (void)detect_sdoor(Ind);
				break;
			}

            case SSPELL_CONFUSE_MONSTER:
			{
                p_ptr->current_spell = SSPELL_CONFUSE_MONSTER;
                get_aim_dir(Ind);
                return;
			}

            case SSPELL_DETECT_OBJECT:
			{
                (void)detect_object(Ind);
				break;
			}

            case SSPELL_NOXIOUS_CLOUD:
			{
                p_ptr->current_spell = SSPELL_NOXIOUS_CLOUD;
				get_aim_dir(Ind);
				return;
			}

            case SSPELL_TELEPORT_SELF:
			{
                msg_format_near(Ind, "%s teleports away!", p_ptr->name);
                teleport_player(Ind, plev * 5);
				break;
			}

            case SSPELL_BEAM_LIGHT:
			{				
                p_ptr->current_spell = SSPELL_BEAM_LIGHT;
				get_aim_dir(Ind);
				return;
			}

            case SSPELL_SLEEP_MONSTER:
			{
                p_ptr->current_spell = SSPELL_SLEEP_MONSTER;
                get_aim_dir(Ind);
                return;
			}

            case SSPELL_LIGHTNING_BOLT:
			{
                p_ptr->current_spell = SSPELL_LIGHTNING_BOLT;
                get_aim_dir(Ind);
                return;
			}

            case SSPELL_STONE_MUD:
			{
                p_ptr->current_spell = SSPELL_STONE_MUD;
                get_aim_dir(Ind);
                return;
			}

            case SSPELL_FROST_BOLT:
			{
                p_ptr->current_spell = SSPELL_FROST_BOLT;
                get_aim_dir(Ind);
                return;
			}

            case SSPELL_WRAITH_FORM:
			{
                set_tim_wraith(Ind, p_ptr->tim_wraith + 10 + plev + randint(20));
				break;
			}

            case SSPELL_WRAITH_FORM + 64:
			{
		/* projected wraithform */
                p_ptr->current_spell = SSPELL_WRAITH_FORM;
				get_aim_dir(Ind);
				return;
			}

            case SSPELL_ETHEREAL_EYE:
			{
                map_area(Ind);
                set_tim_invis(Ind, p_ptr->tim_invis + 20 + randint(20));
				break;
			}

	    case SSPELL_FIRE_BOLT:
			{
                p_ptr->current_spell = SSPELL_FIRE_BOLT;
				get_aim_dir(Ind);
				return;
			}

            case SSPELL_IDENTIFY_ITEM:
			{
                (void)ident_spell(Ind);
				break;
			}

            case SSPELL_RADIATE_FEAR:
			{
                fire_ball(Ind, GF_TURN_ALL, 0, 20, 10);
				break;
			}

            case SSPELL_HASTE_SELF:
			{
                msg_format_near(Ind, "%s starts moving faster.", p_ptr->name);
                if (!p_ptr->fast)
                {
                    (void)set_fast(Ind, randint(20 + (plev)) + plev);
                }
                else
                {
                    (void)set_fast(Ind, p_ptr->fast + randint(5));
                }
				break;
			}

            case SSPELL_HASTE_SELF + 64:
			{
		/* projected haste */
                p_ptr->current_spell = SSPELL_HASTE_SELF;
                get_aim_dir(Ind);
                return;
			}

	    case SSPELL_ELEMENTAL_BOLT:
			{
                p_ptr->current_spell = SSPELL_ELEMENTAL_BOLT;
                get_aim_dir(Ind);
                return;
			}

            case SSPELL_TELEPORT_AWAY:
			{
                p_ptr->current_spell = SSPELL_TELEPORT_AWAY;
                get_aim_dir(Ind);
                return;
			}

            case SSPELL_CREATE_FOOD:
			{
                (void)set_food(Ind, PY_FOOD_MAX - 1);
				break;
			}

            case SSPELL_MEDITATE_MANA:
			{
                set_tim_meditation(Ind, p_ptr->tim_meditation + 20 + randint(20));
				break;
			}

            case SSPELL_RECHARGE_ITEM:
			{
                (void)recharge(Ind, 40);
				break;
			}

            case SSPELL_FIRE_STORM:
			{
                p_ptr->current_spell = SSPELL_FIRE_STORM;
				get_aim_dir(Ind);
				return;
			}

            case SSPELL_FORCE_SHIELD:
			{
                msg_format_near(Ind, "%s forms a mystic shield.", p_ptr->name);
                if (!p_ptr->shield)
                        (void)set_shield(Ind, p_ptr->shield + randint(20) + 30);
                else
			(void)set_shield(Ind, p_ptr->shield + 10);
				break;
			}

            case SSPELL_FORCE_SHIELD + 64:
			{
		/* projected shield */
                p_ptr->current_spell = SSPELL_FORCE_SHIELD;
                get_aim_dir(Ind);
                return;
            }

            case SSPELL_SUN_FIRE:
            {
                p_ptr->current_spell = SSPELL_SUN_FIRE;
                get_aim_dir(Ind);
                return;
            }

            case SSPELL_SAFE_GUARD:
            {
                int x, y, rad = 2 + (plev / 20);
                	
		msg_format_near(Ind, "%s lays down some glyphs.", p_ptr->name);
                	
                for (x = p_ptr->px - rad; x <= p_ptr->px + rad; x++)
                for (y = p_ptr->py - rad; y <= p_ptr->py + rad; y++)
                {
                	/* First we must be in the dungeon */
                	if (!in_bounds(p_ptr->dun_depth, y, x)) continue;
                		
                	/* Is it a naked grid ? */
                	if (!cave_naked_bold(p_ptr->dun_depth, y, x)) continue;
                		
                	/* Beware of the houses in town */
                	if ((p_ptr->dun_depth <= 0) && (cave[p_ptr->dun_depth][y][x].info & CAVE_ICKY)) continue;
                		
                	/* Now we want a circle */
                	if (distance(y, x, p_ptr->py, p_ptr->px) != rad) continue;
                		
                	/* Everything ok ? then put a glyph */
			cave[p_ptr->dun_depth][y][x].feat = FEAT_GLYPH;
			note_spot(Ind, y, x);
			everyone_lite_spot(p_ptr->dun_depth, y, x);
                }
		break;
            }

            case SSPELL_CREATE_WALLS:
            {
                fire_ball(Ind, GF_STONE_WALL, 0, 1, 1 + (plev / 40));
                break;
            }

            case SSPELL_CREATE_STAIRS:
            {
                (void)stair_creation(Ind);
                break;
            }

            case SSPELL_ELEM_SHIELD:
            {
                (void)set_oppose_acid(Ind, p_ptr->oppose_acid + randint(20) + 20);
                (void)set_oppose_elec(Ind, p_ptr->oppose_elec + randint(20) + 20);
                (void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(20) + 20);
                (void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(20) + 20);
                (void)set_oppose_pois(Ind, p_ptr->oppose_pois + randint(20) + 20);
                break;
            }

            case SSPELL_WORD_RECALL:
            {
		if (cfg_ironman)
                	msg_print(Ind, "The air about you becomes charged... but only for a moment...");
                else if (!p_ptr->word_recall)
                {
                    set_recall_depth(p_ptr, o_ptr);
                    p_ptr->word_recall = rand_int(20) + 15;
                    msg_print(Ind, "The air about you becomes charged...");
                }
                else
                {
                    p_ptr->word_recall = 0;
                    msg_print(Ind, "A tension leaves the air around you...");
                }
                break;
            }

            case SSPELL_WORD_RECALL + 64:
            {
		/* projected recall */
                p_ptr->current_spell = SSPELL_WORD_RECALL;
                get_aim_dir(Ind);
                return;
            }

            case SSPELL_MANA_SHIELD:
            {
                (void)set_tim_manashield(Ind, p_ptr->tim_manashield + randint(10) + 8);
                break;
            }

            case SSPELL_EARTH_QUAKE:
            {
                msg_format_near(Ind, "%s casts a spell, and the ground shakes!", p_ptr->name);
                earthquake(p_ptr->dun_depth, p_ptr->py, p_ptr->px, 5);
                break;
            }

            case SSPELL_POLY_OTHER:
            {
                p_ptr->current_spell = SSPELL_POLY_OTHER;
                get_aim_dir(Ind);
                return;
            }

            case SSPELL_CHAOS_BLAST:
            {
                fire_ball(Ind, GF_CHAOS, 0, 100 + (3 * plev), 5);
                break;
            }

            case SSPELL_WIPE_AREA:
            {
                msg_format_near(Ind, "%s wrecks havoc.", p_ptr->name);
                wipe_spell(p_ptr->dun_depth, p_ptr->py, p_ptr->px, 10);
                take_hit(Ind, 100, "the strain of casting the Wipe spell");
                break;
            }

            case SSPELL_MANA_BLAST:
            {
                fire_ball(Ind, GF_MANA, 0, 600 + (2 * plev), 3);
                take_hit(Ind, 90, "the strain of casting mana blasts");
                break;
            }

            case SSPELL_WORD_DESTRUCTION:
            {
                msg_format_near(Ind, "%s unleashes great power!", p_ptr->name);
                destroy_area(p_ptr->dun_depth, p_ptr->py, p_ptr->px, 15, TRUE);
                break;
            }

            case SSPELL_MIND_VISION:
            {
                int Ind2 = get_player(Ind, o_ptr);

		if (!Ind2) break;

		if (Ind == Ind2)
		{
			msg_print(Ind, "You cannot link to your own mind.");
			break;
		}
		if (p_ptr->esp_link)
		{
			msg_print(Ind, "Your mind is already linked.");
			break;
		}
		if (Players[Ind2]->esp_link)
		{
			msg_format(Ind, "%s's mind is already linked.", Players[Ind2]->name);
			break;
		}

		msg_format(Ind2, "%s infiltrates your mind.", p_ptr->name);
		msg_format(Ind, "You infiltrate %s's mind.", Players[Ind2]->name);
		p_ptr->esp_link = Players[Ind2]->id;
		p_ptr->esp_link_type = LINK_DOMINANT;
		p_ptr->esp_link_flags = 0;
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
		p_ptr->update |= (PU_BONUS | PU_VIEW | PU_MANA | PU_HP);
		p_ptr->redraw |= (PR_BASIC | PR_EXTRA | PR_MAP);

		Players[Ind2]->esp_link = p_ptr->id;
		Players[Ind2]->esp_link_type = LINK_DOMINATED;
		Players[Ind2]->esp_link_flags = LINKF_VIEW;
		Players[Ind2]->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
		Players[Ind2]->update |= (PU_BONUS | PU_VIEW | PU_MANA | PU_HP);
		Players[Ind2]->redraw |= (PR_BASIC | PR_EXTRA | PR_MAP);
		break;
            }

            case SSPELL_TELE_OBJECT:
            {
                telekinesis(Ind, o_ptr);
                break;
            }

            case SSPELL_SELF_SCAN:
            {
                self_knowledge(Ind);
                break;
            }

            case SSPELL_IDENTIFY_FULL:
            {
                identify_fully(Ind);
                break;
            }

            case SSPELL_ENLIGHT_LEVEL:
            {
                wiz_lite(Ind);
                break;
            }

            case SSPELL_PLASMA_BLAST:
            {
                fire_ball(Ind, GF_PLASMA, 0, 400 + (2 * plev), 4);
                break;
            }

            case SSPELL_ANNIHIL_BOLT:
            {
                p_ptr->current_spell = SSPELL_ANNIHIL_BOLT;
                get_aim_dir(Ind);
                return;
            }

            case SSPELL_OBLIVION_BLAST:
            {
                (void)mass_genocide(Ind);
                break;
            }

            case SSPELL_TIDAL_WAVE:
            {
                p_ptr->current_spell = SSPELL_TIDAL_WAVE;
                get_aim_dir(Ind);
                return;
            }

            case SSPELL_ANARCHY_FORCE:
            {
                int x, y;

                for (y = 0; y < 5; y++)
                {
			x = randint(10);
                        while (x == 5) x = randint(10);
                        (void)fire_ball(Ind, GF_CHAOS, x, 250 + plev, 3);
                }
                break;
            }

            case SSPELL_MANA_STRIKE:
            {
                p_ptr->current_spell = SSPELL_MANA_STRIKE;
                get_aim_dir(Ind);
                return;
            }
        }

        /* A spell was cast */
        if (!((j < 32) ?
              (p_ptr->spell_worked1 & (1L << j)) :
              (p_ptr->spell_worked2 & (1L << (j - 32)))))
        {
            int e = s_ptr->sexp;

            /* The spell worked */
            if (j < 32)
            {
                p_ptr->spell_worked1 |= (1L << j);
            }
            else
            {
                p_ptr->spell_worked2 |= (1L << (j - 32));
            }

            /* Gain experience */
            gain_exp(Ind, e * s_ptr->slevel);

            /* Fix the spell info */
            p_ptr->window |= PW_SPELL;
        }
    }

    /* Take a turn */
    p_ptr->energy -= level_speed(p_ptr->dun_depth);

    /* Sufficient mana */
    if (s_ptr->smana <= p_ptr->csp)
    {
        /* Use some mana */
        p_ptr->csp -= s_ptr->smana;
    }

    /* Over-exert the player */
    else
    {
        int oops = s_ptr->smana - p_ptr->csp;

        /* No mana left */
        p_ptr->csp = 0;
        p_ptr->csp_frac = 0;

        /* Message */
        msg_print(Ind, "You faint from the effort!");

        /* Hack -- Bypass free action */
        (void)set_paralyzed(Ind, p_ptr->paralyzed + randint(5 * oops + 1));

        /* Damage CON (possibly permanently) */
        if (rand_int(100) < 50)
        {
            bool perm = (rand_int(100) < 25);

            /* Message */
            msg_print(Ind, "You have damaged your health!");

            /* Reduce constitution */
            (void)dec_stat(Ind, A_CON, 15 + randint(10), perm);
        }
    }

    /* Redraw mana */
    p_ptr->redraw |= (PR_MANA);

    /* Window stuff */
    p_ptr->window |= (PW_PLAYER);
}


/*
 * Finish casting a spell that required a direction --KLJ--
 */
void do_cmd_sorc_aux(int Ind, int dir)
{
    player_type *p_ptr = Players[Ind];

    int plev = p_ptr->lev;
    int beam = ((p_ptr->pclass == CLASS_SORCEROR) ? plev : (plev / 2));

    magic_type *s_ptr = &p_ptr->mp_ptr->info[p_ptr->current_spell];

    /* Only fire in direction 5 if we have a target */
    if ((dir == 5) && !target_okay(Ind))
    {
        /* Reset current spell */
        p_ptr->current_spell = -1;

        /* Done */
        return;
    }

    /* We assume that the spell can be cast, and so forth */
    switch(p_ptr->current_spell)
    {
        case SSPELL_MAGIC_MISSILE:
        {
            msg_format_near(Ind, "%s fires a magic missile.", p_ptr->name);
            fire_bolt_or_beam(Ind, beam-10, GF_MISSILE, dir,
                        damroll(3 + ((plev - 1) / 5), 4 + (plev / 10)));
            break;
        }

        case SSPELL_CONFUSE_MONSTER:
        {
            msg_format_near(Ind, "%s looks mesmerizing.", p_ptr->name);
	    confuse_monster(Ind, dir, ( plev * 3) / 2);
            break;
        }

        case SSPELL_NOXIOUS_CLOUD:
        {
            msg_format_near(Ind, "%s casts a noxious cloud.", p_ptr->name);
            fire_ball(Ind, GF_POIS, dir,
                10 + (plev / 2), 2);
            break;
        }

        case SSPELL_BEAM_LIGHT:
        {
            msg_print(Ind, "A line of blue shimmering light appears.");
            lite_line(Ind, dir);
            break;
        }

        case SSPELL_SLEEP_MONSTER:
        {
            sleep_monster(Ind, dir);
            break;
        }

        case SSPELL_LIGHTNING_BOLT:
        {
            msg_format_near(Ind, "%s fires a lightning bolt.", p_ptr->name);
            fire_bolt_or_beam(Ind, beam-10, GF_ELEC, dir,
                damroll(5+((plev-5)/4), 8));
            break;
        }

        case SSPELL_STONE_MUD:
        {
            (void)wall_to_mud(Ind, dir);
            break;
        }

        case SSPELL_FROST_BOLT:
        {
            msg_format_near(Ind, "%s fires a frost bolt.", p_ptr->name);
            fire_bolt_or_beam(Ind, beam-10, GF_COLD, dir,
                damroll(6+((plev-5)/4), 8));
            break;
        }

        case SSPELL_WRAITH_FORM:
        {
            project_hook(Ind, GF_WRAITH_PLAYER, dir, 10 + plev + randint(20), PROJECT_STOP | PROJECT_KILL);
            break;
        }

        case SSPELL_FIRE_BOLT:
        {
            msg_format_near(Ind, "%s casts a fire bolt.", p_ptr->name);
            fire_bolt_or_beam(Ind, beam, GF_FIRE, dir,
                damroll(9+((plev-5)/4), 8));
            break;
        }

        case SSPELL_HASTE_SELF:
        {
            project_hook(Ind, GF_SPEED_PLAYER, dir, randint(20 + (plev)) + plev, PROJECT_STOP | PROJECT_KILL);
            break;
        }

        case SSPELL_ELEMENTAL_BOLT:
        {
            msg_format_near(Ind, "%s fires an elemental bolt.", p_ptr->name);
	    fire_bolt_or_beam(Ind, beam, GF_FIRE, dir, damroll(5+((plev-5)/4), 8));
            fire_bolt_or_beam(Ind, beam, GF_COLD, dir, damroll(5+((plev-5)/4), 8));
            fire_bolt_or_beam(Ind, beam, GF_ACID, dir, damroll(5+((plev-5)/4), 8));
            fire_bolt_or_beam(Ind, beam, GF_ELEC, dir, damroll(5+((plev-5)/4), 8));
            break;
        }

        case SSPELL_TELEPORT_AWAY:
        {
            (void)fire_beam(Ind, GF_AWAY_ALL, dir, plev);
            break;
        }

        case SSPELL_FIRE_STORM:
        {
            msg_format_near(Ind, "%s invokes a firestorm.", p_ptr->name);
            (void)fire_ball(Ind, GF_FIRE, dir, 100 + (3 * plev), 2);
            break;
        }

        case SSPELL_FORCE_SHIELD:
        {
            project_hook(Ind, GF_SHIELD_PLAYER, dir, randint(20) + 30, PROJECT_STOP | PROJECT_KILL);
            break;
        }

        case SSPELL_SUN_FIRE:
        {
            msg_format_near(Ind, "%s invokes the sunfire.", p_ptr->name);
            (void)fire_ball(Ind, GF_LITE, dir, 130 + (plev * 5 / 2), 3);
            break;
        }

        case SSPELL_WORD_RECALL:
        {
            project_hook(Ind, GF_RECALL_PLAYER, dir, randint(20) + 15, PROJECT_STOP | PROJECT_KILL);
            break;
        }
        
	case SSPELL_POLY_OTHER:
        {
            (void)poly_monster(Ind, dir);
            break;
        }

        case SSPELL_ANNIHIL_BOLT:
        {
            msg_format_near(Ind, "%s fires an annihilation bolt.", p_ptr->name);
            (void)fire_bolt(Ind, GF_MISSILE, dir, 500 + (2 * plev));
            break;
        }

	case SSPELL_TIDAL_WAVE:
        {
            msg_format_near(Ind, "%s invokes a tidal wave.", p_ptr->name);
            (void)fire_ball(Ind, GF_WATER, dir, 450 + (2 * plev), 3);
            break;
        }

	case SSPELL_MANA_STRIKE:
        {
            msg_format_near(Ind, "%s invokes a mana storm.", p_ptr->name);
            (void)fire_ball(Ind, GF_MANA, dir, 800, 0);
            break;
        }

        default:  /* For some reason we got called for a spell that */
        {         /* doesn't require a direction */
            msg_print(Ind, "SERVER ERROR: do_cmd_sorc_aux() called for non-directional spell!");
            p_ptr->current_spell = -1;
            return;
        }
    }	

    if (!((p_ptr->current_spell < 32) ?
        (p_ptr->spell_worked1 & (1L << p_ptr->current_spell)) :
        (p_ptr->spell_worked2 & (1L << (p_ptr->current_spell - 32)))))
    {
        int e = s_ptr->sexp;

        if (p_ptr->current_spell < 32)
        {
            p_ptr->spell_worked1 |= (1L << p_ptr->current_spell);
        }
        else
        {
            p_ptr->spell_worked2 |= (1L << (p_ptr->current_spell - 32));
        }

        gain_exp(Ind, e * s_ptr->slevel);

        /* Fix the spell info */
        p_ptr->window |= PW_SPELL;
    }

    p_ptr->energy -= level_speed(p_ptr->dun_depth);

    if (s_ptr->smana <= p_ptr->csp)
    {
        /* Use some mana */
        p_ptr->csp -= s_ptr->smana;
    }

    /* Over-exert the player */
    else
    {
        int oops = s_ptr->smana - p_ptr->csp;

        /* No mana left */
        p_ptr->csp = 0;
        p_ptr->csp_frac = 0;

        /* Message */
        msg_print(Ind, "You faint from the effort!");

        /* Hack -- bypass free action */
        (void)set_paralyzed(Ind, p_ptr->paralyzed + randint(5 * oops + 1));

        /* Damage CON (possibly permanently) */
        if (rand_int(100) < 50)
        {
            bool perm = (rand_int(100) < 25);

            /* Message */
            msg_print(Ind, "You have damaged your health!");

            /* Reduce constitution */
            (void)dec_stat(Ind, A_CON, 15 + randint(10), perm);
        }
    }

    /* Reset current spell */
    p_ptr->current_spell = -1;

    /* Resend mana */
    p_ptr->redraw |= (PR_MANA);

    /* Window stuff */
    p_ptr->window |= (PW_PLAYER);
}

#endif
    

/*
 * Brand the current weapon
 */
static void brand_weapon(int Ind)
{
    player_type *p_ptr = Players[Ind];

    object_type *o_ptr;

    o_ptr = &p_ptr->inventory[INVEN_WIELD];

    /* you can never modify artifacts / ego-items */
    /* you can never modify broken / cursed items */
    if ((o_ptr->k_idx) &&
        (!artifact_p(o_ptr)) && (!ego_item_p(o_ptr)) &&
        (!broken_p(o_ptr)) && (!cursed_p(o_ptr)))
    {
        cptr act = NULL;

        char o_name[80];

        if (rand_int(100) < 25)
        {
            act = "is covered in a fiery shield!";
            o_ptr->name2 = EGO_BRAND_FIRE;
        }

        else
        {
            act = "glows deep, icy blue!";
            o_ptr->name2 = EGO_BRAND_COLD;
        }

        object_desc(Ind, o_name, o_ptr, FALSE, 0);

        msg_format(Ind, "Your %s %s", o_name, act);

        enchant(Ind, o_ptr, rand_int(3) + 4, ENCH_TOHIT | ENCH_TODAM);
    }

    else
    {
        if (flush_failure) flush();
        msg_print(Ind, "The Branding failed.");
    }
}


/*
 * Pray a prayer
 *
 * See above for an explanation of the weirdness in this function --KLJ--
 */
void do_cmd_pray(int Ind, int book, int spell)
{
    player_type *p_ptr = Players[Ind];

    int item, sval, j, chance, i;
    int plev = p_ptr->lev;

    object_type	*o_ptr;

    magic_type  *s_ptr;

    byte spells[64], num = 0;

    /* Must use prayer books */
    if (p_ptr->mp_ptr->spell_book != TV_PRAYER_BOOK)
    {
        msg_print(Ind, "Pray hard enough and your prayers may be answered.");
        return;
    }

    /* Must have lite */
    if (p_ptr->blind || no_lite(Ind))
    {
        msg_print(Ind, "You cannot see!");
        return;
    }

    /* Must not be confused */
    if (p_ptr->confused)
    {
        msg_print(Ind, "You are too confused!");
        return;
    }


    /* Restrict choices */
    item_tester_tval = p_ptr->mp_ptr->spell_book;

    item = book;

    /* Get the item (in the pack) */
    if (item >= 0)
    {
        o_ptr = &p_ptr->inventory[item];
    }

    /* Get the item (on the floor) */
    else
    {
        o_ptr = &o_list[0 - item];
    }

    if (o_ptr->tval != p_ptr->mp_ptr->spell_book)
    {
        msg_print(Ind, "SERVER ERROR: Tried to pray prayer from bad book!");
        return;
    }
        if( check_guard_inscription( o_ptr->note, 'p' )) {
                msg_print(Ind, "The item's inscription prevents it");
                return;
        };

    /* Access the item's sval */
    sval = o_ptr->sval;

    for (i = 0; i < 64; i++)
    {
        /* Check for this spell */
        if ((i < 32) ?
            (spell_flags[p_ptr->mp_ptr->spell_type][sval][0] & (1L << i)) :
            (spell_flags[p_ptr->mp_ptr->spell_type][sval][1] & (1L << (i - 32))))
        {
            /* Collect this spell */
            spells[num++] = i;
        }
    }
    
    /* OK, this is a unsightly kludge to get some extra (heal other) 
    prayers without modifying the spell_flags... but it should work for 
    now. -AD- */
    
    /* normal prayers */
    if (spell < 64) j = spells[spell];
    
    /* heal other prayers */
    else j = spells[spell-64];
    
    if (!spell_okay(Ind, j, 1))
    {
        msg_print(Ind, "You cannot pray that prayer.");
        return;
    }
    
    /* get the spell info */
    s_ptr = &p_ptr->mp_ptr->info[j];
    
    /* Check mana */
    if (s_ptr->smana > p_ptr->csp)
    {
        msg_print(Ind, "You do not have enough mana.");
        return;
    }

    /* Spell failure chance */
    chance = spell_chance(Ind, j);

    /* Check for failure */
    if (rand_int(100) < chance)
    {
        if (flush_failure) flush();
        msg_print(Ind, "You failed to concentrate hard enough!");
    }

    /* Success */
    else
    {
        if (spell >= 64) j += 64;
        switch (j)
        {
            case PSPELL_DETECT_EVIL:
            {
                (void)detect_evil(Ind);
                break;
            }

            case PSPELL_CURE_LIGHT:
            {
                (void)hp_player(Ind, damroll(2, 10));
                (void)set_cut(Ind, p_ptr->cut - 10);				
                break;
            }

            /* cure light wounds projectile */
            case PSPELL_CURE_LIGHT+64:
            {				
                p_ptr->current_spell = PSPELL_CURE_LIGHT;
                get_aim_dir(Ind);
                
                return;
            }

            case PSPELL_HERO_BLESS:
            {
                (void)set_blessed(Ind, p_ptr->blessed + randint(12) + 12);
                break;
            }

            case PSPELL_REMOVE_FEAR:
            {
                (void)set_afraid(Ind, 0);
                break;
            }

            case PSPELL_CALL_LIGHT:
            {
                msg_format_near(Ind, "%s calls light.", p_ptr->name);
                (void)lite_area(Ind, damroll(2, (plev / 2)), (plev / 10) + 1);
                break;
            }

            case PSPELL_FIND_TRAPS:
            {
                (void)detect_trap(Ind);
                break;
            }

            case PSPELL_DETECT_FEATURES:
            {
                (void)detect_sdoor(Ind);
                break;
            }

            case PSPELL_SLOW_POISON:
            {
                (void)set_poisoned(Ind, p_ptr->poisoned / 2);
                break;
            }

            case PSPELL_SCARE_MONSTER:
            {
                p_ptr->current_spell = PSPELL_SCARE_MONSTER;
                get_aim_dir(Ind);
                return;
            }

            case PSPELL_TELEPORT_PORTAL:
            {
                msg_format_near(Ind, "%s blinks away!", p_ptr->name);
                teleport_player(Ind, plev * 3);
                break;
            }

            case PSPELL_CURE_SERIOUS:
            {
                (void)hp_player(Ind, damroll(4, 10));
                (void)set_cut(Ind, (p_ptr->cut / 2) - 20);
                break;
            }

            /* cure serious wounds projectile */
            case PSPELL_CURE_SERIOUS+64:
            {				
                p_ptr->current_spell = PSPELL_CURE_SERIOUS;
                get_aim_dir(Ind);
                return;
            }


            case PSPELL_HERO_CHANT:
            {
                (void)set_blessed(Ind, p_ptr->blessed + randint(24) + 24);
                break;
            }

            case PSPELL_SLEEP_SANCTUARY:
            {
                (void)sleep_monsters_touch(Ind);
				break;
			}

            case PSPELL_SAT_HUNGER:
            {
                (void)set_food(Ind, PY_FOOD_MAX - 1);
                break;
            }

            case PSPELL_REMOVE_CURSE:
            {
                remove_curse(Ind);
                break;
            }

            case PSPELL_RESIST_LOWS:
            {
                (void)set_oppose_fire(Ind, p_ptr->oppose_fire + randint(10) + 10);
                (void)set_oppose_cold(Ind, p_ptr->oppose_cold + randint(10) + 10);
                break;
            }

            case PSPELL_CURE_POISON:
            {
                (void)set_poisoned(Ind, 0);
                break;
            }

            case PSPELL_ORB_DRAINING:
            {
                p_ptr->current_spell = PSPELL_ORB_DRAINING;
                get_aim_dir(Ind);
                return;
            }

            case PSPELL_CURE_CRITICAL:
            {
                (void)hp_player(Ind, damroll(6, 10));
                (void)set_cut(Ind, 0);
                break;
            }

            /* cure critical wounds projectile */
            case PSPELL_CURE_CRITICAL+64:
            {
                p_ptr->current_spell = PSPELL_CURE_CRITICAL;
                get_aim_dir(Ind);
                return;
            }

            case PSPELL_SENSE_INVISIBLE:
            {
                (void)set_tim_invis(Ind, p_ptr->tim_invis + randint(24) + 24);
                break;
            }

            case PSPELL_PROTECTION_EVIL:
            {
                (void)set_protevil(Ind, p_ptr->protevil + randint(25) + 3 * p_ptr->lev);
                break;
            }

            case PSPELL_EARTH_QUAKE:
            {
                msg_format_near(Ind, "%s murmurs, and the ground shakes!", p_ptr->name);
                earthquake(p_ptr->dun_depth, p_ptr->py, p_ptr->px, 10);
                break;
            }

            case PSPELL_SENSE_MAP:
            {
                map_area(Ind);
                break;
            }

            case PSPELL_CURE_MORTAL:
            {
                (void)hp_player(Ind, damroll(10, 10));
                (void)set_stun(Ind, 0);
                (void)set_cut(Ind, 0);
                break;
            }
	    /* Cure mortal wounds projectile */
            case PSPELL_CURE_MORTAL+64:
            {
                p_ptr->current_spell = PSPELL_CURE_MORTAL;
                get_aim_dir(Ind);
                return;
            }

            case PSPELL_TURN_UNDEAD:
            {
                (void)turn_undead(Ind);
                break;
            }

            case PSPELL_HERO_PRAYER:
            {
                (void)set_blessed(Ind, p_ptr->blessed + randint(48) + 48);
                break;
            }

            case PSPELL_DISPEL_UNDEAD:
            {
                (void)dispel_undead(Ind, plev * 3);
                break;
            }

            case PSPELL_CURE_HEAL:
            {
                (void)hp_player(Ind, 300);
                (void)set_stun(Ind, 0);
                (void)set_cut(Ind, 0);
                break;
            }
            /* Heal other projectile */
            case PSPELL_CURE_HEAL+64:
            {
                p_ptr->current_spell = PSPELL_CURE_HEAL;
                get_aim_dir(Ind);
                return;
            }

            case PSPELL_DISPEL_EVIL:
            {
                (void)dispel_evil(Ind, plev * 3);
                break;
            }

            case PSPELL_GLYPH_WARDING:
            {
                msg_format_near(Ind, "%s lays down a glyph of warding.", p_ptr->name);
                warding_glyph(Ind);
                break;
            }

            case PSPELL_HOLY_WORD:
			{
				(void)dispel_evil(Ind, plev * 4);
				(void)hp_player(Ind, 1000);
				(void)set_afraid(Ind, 0);
				(void)set_poisoned(Ind, 0);
				(void)set_stun(Ind, 0);
				(void)set_cut(Ind, 0);
				break;
			}

            case PSPELL_DETECT_MONSTERS:
			{
				(void)detect_creatures(Ind);
				break;
			}

            case PSPELL_DETECT_ALL:
			{
				(void)detection(Ind);
				break;
			}

            case PSPELL_IDENTIFY_ITEM:
			{
				(void)ident_spell(Ind);
				break;
			}

            case PSPELL_PROBE_MONSTER:
			{
				(void)probing(Ind);
				break;
			}

            case PSPELL_ENLIGHT_LEVEL:
			{
				wiz_lite(Ind);
				break;
			}

            case PSPELL_CURE_CRIT2:
			{
                (void)hp_player(Ind, damroll(6, 10));
				(void)set_cut(Ind, 0);
				break;
			}
	    /* cure critical wounds projectile */
            case PSPELL_CURE_CRIT2+64:
            {
                p_ptr->current_spell = PSPELL_CURE_CRIT2;
                get_aim_dir(Ind);
                return;
            }

            case PSPELL_CURE_HEALING:
			{
                (void)hp_player(Ind, 100);
				(void)set_stun(Ind, 0);
				(void)set_cut(Ind, 0);
				break;
			}
	    /* Healing projectile */
            case PSPELL_CURE_HEALING+64:
            {
                p_ptr->current_spell = PSPELL_CURE_HEALING;
                get_aim_dir(Ind);
                return;
            }

            case PSPELL_EXTRA_HEALING:
			{
				(void)hp_player(Ind, 2000);
				(void)set_stun(Ind, 0);
				(void)set_cut(Ind, 0);
				break;
			}

			/* heal other spell */
            case PSPELL_EXTRA_HEALING+64:
			{
                p_ptr->current_spell = PSPELL_EXTRA_HEALING;
				get_aim_dir(Ind);
				return;
			}

			/* should ressurect ghosts now */
            case PSPELL_RESTORE_STATS:
			{
				/* if we didnt ressurect anybody */
				if (!do_scroll_life(Ind))
				{
					(void)do_res_stat(Ind, A_STR);
					(void)do_res_stat(Ind, A_INT);
					(void)do_res_stat(Ind, A_WIS);
					(void)do_res_stat(Ind, A_DEX);
					(void)do_res_stat(Ind, A_CON);
					(void)do_res_stat(Ind, A_CHR);
				}
				break;
			}

            case PSPELL_RESTORE_XP:
			{
				if (!do_restoreXP_other(Ind))
				{
					(void)restore_level(Ind);
				}
				break;
			}

            case PSPELL_DISPEL_UNDEAD2:
			{
				
				(void)dispel_undead(Ind, plev * 4);
				break;
			}

            case PSPELL_DISPEL_EVIL2:
			{
				(void)dispel_evil(Ind, plev * 4);
				break;
			}

            case PSPELL_BANISH_EVIL:
			{
				if (banish_evil(Ind, 100))
				{
					msg_print(Ind, "The power of your god banishes evil!");
				}
				break;
			}

            case PSPELL_WORD_DESTRUCTION:
			{
				msg_format_near(Ind, "%s unleashs a spell of great power!", p_ptr->name);
				destroy_area(p_ptr->dun_depth, p_ptr->py, p_ptr->px, 15, TRUE);
				break;
			}

            case PSPELL_ANNIHILAT_BOLT:
			{
                p_ptr->current_spell = PSPELL_ANNIHILAT_BOLT;
				get_aim_dir(Ind);
				return;
			}

            case PSPELL_DESTROY_WAYS:
			{
				(void)destroy_doors_touch(Ind);
				break;
			}

            case PSPELL_RECHARGE_ITEM:
			{
				(void)recharge(Ind, 15);
				break;
			}

            case PSPELL_DISPEL_CURSE:
			{
				(void)remove_all_curse(Ind);
				break;
			}

            case PSPELL_ENCHANT_WEAPON:
			{
				(void)enchant_spell(Ind, rand_int(4) + 1, rand_int(4) + 1, 0);
				break;
			}

            case PSPELL_ENCHANT_ARMOR:
			{
				(void)enchant_spell(Ind, 0, 0, rand_int(3) + 2);
				break;
			}

            case PSPELL_ELEM_BRAND:
			{
				brand_weapon(Ind);
				break;
			}

            case PSPELL_TELEPORT_BLINK:
			{
				msg_format_near(Ind, "%s blinks away!", p_ptr->name);
				teleport_player(Ind, 10);
				break;
			}

            case PSPELL_TELEPORT_SELF:
			{
				msg_format_near(Ind, "%s teleports away!", p_ptr->name);
				teleport_player(Ind, plev * 8);
				break;
			}

            case PSPELL_TELEPORT_OTHER:
			{
				p_ptr->current_spell = 54;
				get_aim_dir(Ind);
				return;
			}

            case PSPELL_TELEPORT_LEVEL:
			{
				(void)teleport_player_level(Ind);
				break;
			}

            case PSPELL_WORD_RECALL:
			{
		if (cfg_ironman)
				msg_print(Ind, "The air about you becomes charged... but only for a moment...");
                else if (p_ptr->word_recall == 0)
				{
					set_recall_depth(p_ptr, o_ptr);
					p_ptr->word_recall = rand_int(20) + 15;
					msg_print(Ind, "The air about you becomes charged...");
				}
				else
				{
					p_ptr->word_recall = 0;
					msg_print(Ind, "A tension leaves the air around you...");
				}
				break;
			}

            case PSPELL_ALTER_REALITY:
			{
				/* Alter Reality */
				
				/* Which dungeon level are we changing? */
				int Depth = p_ptr->dun_depth;
        
				/* Don't allow this in towns */
				if( (!Depth) || (check_special_level(Depth)) )
					break;
        
				/* Search for players on this depth */
				for (i = 1; i < NumPlayers + 1; i++)
				{
					player_type *p_ptr = Players[i];
					
					/* Only players on this depth */
					if(p_ptr->dun_depth == Depth)
					{
						/* Tell the player about it */
						msg_print(i, "The world changes!");
						p_ptr->new_level_flag = TRUE;
						p_ptr->new_level_method = LEVEL_RAND;
					}
				}

				/* Deallocate the level */
				dealloc_dungeon_level(Depth);
				cave[Depth] = 0;
				
				break;
			}
		}

    if(spell >= 64) j -= 64; 

		/* A prayer was prayed */
		if (!((j < 32) ?
		      (p_ptr->spell_worked1 & (1L << j)) :
		      (p_ptr->spell_worked2 & (1L << (j - 32)))))
		{
			int e = s_ptr->sexp;

			/* The spell worked */
			if (j < 32)
			{
				p_ptr->spell_worked1 |= (1L << j);
			}
			else
			{
				p_ptr->spell_worked2 |= (1L << (j - 32));
			}

			/* Gain experience */
			gain_exp(Ind, e * s_ptr->slevel);

			/* Fix the spell info */
			p_ptr->window |= PW_SPELL;
		}
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Sufficient mana */
	if (s_ptr->smana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= s_ptr->smana;
	}

	/* Over-exert the player */
	else
	{
		int oops = s_ptr->smana - p_ptr->csp;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
		msg_print(Ind, "You faint from the effort!");

		/* Hack -- Bypass free action */
		(void)set_paralyzed(Ind, p_ptr->paralyzed + randint(5 * oops + 1));

		/* Damage CON (possibly permanently) */
		if (rand_int(100) < 50)
		{
			bool perm = (rand_int(100) < 25);

			/* Message */
			msg_print(Ind, "You have damaged your health!");

			/* Reduce constitution */
			(void)dec_stat(Ind, A_CON, 15 + randint(10), perm);
		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
}

void do_cmd_pray_aux(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];

	int plev = p_ptr->lev;
	
	magic_type *s_ptr = &p_ptr->mp_ptr->info[p_ptr->current_spell];

	/* Only fire in direction 5 if we have a target */
	if ((dir == 5) && !target_okay(Ind))
	{
		/* Reset current spell */
		p_ptr->current_spell = -1;

		/* Done */
		return;
	}

	/* We assume that the spell can be cast, and so forth */
	switch(p_ptr->current_spell)
	{
		
        case PSPELL_CURE_LIGHT:
		{
			(void)cure_light_wounds_proj(Ind, dir);
			break;
		}
	
        case PSPELL_SCARE_MONSTER:
		{
			(void)fear_monster(Ind, dir, plev);
			break;
		}

        case PSPELL_CURE_SERIOUS:
		{
			(void)cure_serious_wounds_proj(Ind, dir);
			break;
		}

        case PSPELL_ORB_DRAINING:
		{
			msg_format_near(Ind, "%s fires a holy orb!", p_ptr->name);
			fire_ball(Ind, GF_HOLY_ORB, dir,
				(damroll(3, 6) + plev +
                 (plev / ((p_ptr->pclass == CLASS_PRIEST) ? 2 : 4))),
				 ((plev < 30) ? 2 : 3));
			break;
		}

        case PSPELL_CURE_CRITICAL: 
	case PSPELL_CURE_CRIT2:
		{
			(void)cure_critical_wounds_proj(Ind, dir);
			break;	
		}

	case PSPELL_CURE_MORTAL: 
        {
            (void)cure_mortal_wounds_proj(Ind, dir);
            break;	
        }

        case PSPELL_CURE_HEAL:
		{
			heal_other_proj(Ind,dir);
			break;
		}

	case PSPELL_CURE_HEALING:
	{
            heal_other_lite_proj(Ind,dir);
            break;
        }

        case PSPELL_EXTRA_HEALING:
		{
			heal_other_heavy_proj(Ind,dir);
			break;
		}

        case PSPELL_ANNIHILAT_BOLT:
		{
			drain_life(Ind, dir, 200);
			break;
		}

        case PSPELL_TELEPORT_OTHER:
		{
			(void)teleport_monster(Ind, dir);
			break;
		}

		default:   /* Something went wrong */
		{
			msg_print(Ind, "SERVER ERROR: do_cmd_prayer_aux() called for non-directional prayer!");
			p_ptr->current_spell = -1;
		}
	}

	if (!((p_ptr->current_spell < 32) ?
		(p_ptr->spell_worked1 & (1L << p_ptr->current_spell)) :
		(p_ptr->spell_worked2 & (1L << (p_ptr->current_spell - 32)))))
	{
		int e = s_ptr->sexp;

		if (p_ptr->current_spell < 32)
		{
			p_ptr->spell_worked1 |= (1L << p_ptr->current_spell);
		}
		else
		{
			p_ptr->spell_worked2 |= (1L << (p_ptr->current_spell - 32));
		}

		gain_exp(Ind, e * s_ptr->slevel);

		/* Fix the spell info */
		p_ptr->window |= PW_SPELL;
	}

	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	if (s_ptr->smana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= s_ptr->smana;
	}

	/* Over-exert the player */
	else
	{
		int oops = s_ptr->smana - p_ptr->csp;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
		msg_print(Ind, "You faint from the effort!");

		/* Hack -- bypass free action */
		(void)set_paralyzed(Ind, p_ptr->paralyzed + randint(5 * oops - 1));

		/* Damage CON (possibly permanently) */
		if (rand_int(100) < 50)
		{
			bool perm = (rand_int(100) < 25);

			/* Message */
			msg_print(Ind, "You have damaged your health!");

			/* Reduce constitution */
			(void)dec_stat(Ind, A_CON, 15 + randint(10), perm);
		}
	}

	/* Reset current spell */
	p_ptr->current_spell = -1;

	/* Resend mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
}


/*
 * Send the ghost spell info to the client.
 */
void show_ghost_spells(int Ind)
{
	magic_type *s_ptr;
	int i, j = 0;
	char out_val[80];
	cptr comment = "";

	/* Check each spell */
	for (i = 0; i < 64; i++)
	{
		s_ptr = &ghost_spells[i];

		/* Check for existance */
		if (s_ptr->slevel >= 99) continue;

		/* Format information */
		sprintf(out_val, "  %c) %-30s%2d %4d %3d%%%s",
                I2A(j), spell_names[GHOST_SPELLS][i], s_ptr->slevel, s_ptr->smana, 0, comment);

		/* Send it */
		Send_spell_info(Ind, 0, j, out_val);

		/* Next spell */
		j++;
	}
}

/*
 * Use a ghostly ability. --KLJ--
 */
void do_cmd_ghost_power(int Ind, int ability)
{
	player_type *p_ptr = Players[Ind];
	magic_type *s_ptr = &ghost_spells[ability];
	int plev = p_ptr->lev;
	int i, j = 0;

	/* Check for ghost-ness */
	if (!p_ptr->ghost) return;

	/* Must not be confused */
	if (p_ptr->confused)
	{
		/* Message */
		msg_print(Ind, "You are too confused!");
		return;
	}

	/* Check spells */
	for (i = 0; i < 64; i++)
	{
		s_ptr = &ghost_spells[i];

		/* Check for existance */
		if (s_ptr->slevel >= 99) continue;

		/* Next spell */
		if (j++ == ability) break;
	}

	/* Check for level */
	if (s_ptr->slevel > plev)
	{
		/* Message */
		msg_print(Ind, "You aren't powerful enough to use that ability.");
		return;
	}

	/* Spell effects */
	switch(i)
	{
		case 0:
		{
			teleport_player(Ind, 10);
			break;
		}
		case 1:
		{
			get_aim_dir(Ind);
			p_ptr->current_spell = 1;
			return;
		}
		case 2:
		{
			get_aim_dir(Ind);
			p_ptr->current_spell = 2;
			return;
		}
		case 3:
		{
			teleport_player(Ind, plev * 8);
			break;
		}
		case 4:
		{
			get_aim_dir(Ind);
			p_ptr->current_spell = 4;
			return;
		}
		case 5:
		{
			get_aim_dir(Ind);
			p_ptr->current_spell = 5;
			return;
		}
		case 6:
		{
			get_aim_dir(Ind);
			p_ptr->current_spell = 6;
			return;
		}
	}

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Take some experience */
	p_ptr->max_exp -= s_ptr->slevel * s_ptr->smana;
	p_ptr->exp -= s_ptr->slevel * s_ptr->smana;

	/* Too much can kill you */
	if (p_ptr->exp < 0) take_hit(Ind, 5000, "the strain of ghostly powers");

	/* Check experience levels */
	check_experience(Ind);

	/* Redraw experience */
	p_ptr->redraw |= (PR_EXP);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
}


/*
 * Directional ghost ability
 */
void do_cmd_ghost_power_aux(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];
	int plev = p_ptr->lev;
	magic_type *s_ptr;
	
	/* Verify spell number */
	if (p_ptr->current_spell < 0)
		return;

	/* Acquire spell pointer */
	s_ptr = &ghost_spells[p_ptr->current_spell];

	/* We assume everything is still OK to cast */
	switch (p_ptr->current_spell)
	{
		case 1:
		{
			(void)fear_monster(Ind, dir, plev);
			break;
		}
		case 2:
		{
			confuse_monster(Ind, dir, plev);
			break;
		}
		case 4:
		{
			fire_bolt_or_beam(Ind, plev * 2, GF_NETHER, dir, 50 + damroll(5, 5) + plev);
			break;
		}
		case 5:
		{
			fire_ball(Ind, GF_NETHER, dir, 100 + 2 * plev, 2);
			break;
		}
		case 6:
		{
			fire_ball(Ind, GF_DARK, dir, plev * 5 + damroll(10, 10), 3);
			break;
		}
	}

	/* No more spell */
	p_ptr->current_spell = -1;

	/* Take a turn */
	p_ptr->energy -= level_speed(p_ptr->dun_depth);

	/* Take some experience */
	p_ptr->max_exp -= s_ptr->slevel * s_ptr->smana;
	p_ptr->exp -= s_ptr->slevel * s_ptr->smana;

	/* Too much can kill you */
	if (p_ptr->exp < 0) take_hit(Ind, 5000, "the strain of ghostly powers");

	/* Check experience levels */
	check_experience(Ind);

	/* Redraw experience */
	p_ptr->redraw |= (PR_EXP);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
}
