/* File: obj-desc.c */

/* Purpose: handle object descriptions, mostly string handling code */

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
 * XXX XXX Hack -- note that "TERM_MULTI" is now just "TERM_VIOLET"
 * We will have to find a cleaner method for "MULTI_HUED" later.
 * There were only two multi-hued "flavors" (one potion, one food).
 * Plus five multi-hued "base-objects" (3 dragon scales, one blade
 * of chaos, and one something else).  See the SHIMMER_OBJECTS code
 * in "dungeon.c" and the object color extractor in "cave.c".
 */
#define TERM_MULTI	TERM_VIOLET


/*
 * Max sizes of the following arrays
 */
#define MAX_ROCKS      42       /* Used with rings (min 38) */
#define MAX_AMULETS    26       /* Used with amulets (min 13) */
#define MAX_WOODS      32       /* Used with staffs (min 30) */
#define MAX_METALS     32       /* Used with wands/rods (min 29/28) */
#define MAX_COLORS     60       /* Used with potions (min 60) */
#define MAX_SHROOM     20       /* Used with mushrooms (min 20) */
#define MAX_TITLES     50       /* Used with scrolls (min 48) */
#define MAX_SYLLABLES 158       /* Used with scrolls (see below) */


/*
 * Rings (adjectives and colors)
 */

static cptr ring_adj[MAX_ROCKS] =
{
	"Alexandrite", "Amethyst", "Aquamarine", "Azurite", "Beryl",
	"Bloodstone", "Calcite", "Carnelian", "Corundum", "Diamond",
	"Emerald", "Fluorite", "Garnet", "Granite", "Jade",
	"Jasper", "Lapis Lazuli", "Malachite", "Marble", "Moonstone",
	"Onyx", "Opal", "Pearl", "Quartz", "Quartzite",
	"Rhodonite", "Ruby", "Sapphire", "Tiger Eye", "Topaz",
	"Turquoise", "Zircon", "Platinum", "Bronze", "Gold",
	"Obsidian", "Silver", "Tortoise Shell", "Mithril", "Jet",
	"Engagement", "Adamantite"
};

static byte ring_col[MAX_ROCKS] =
{
	TERM_GREEN, TERM_VIOLET, TERM_L_BLUE, TERM_L_BLUE, TERM_L_GREEN,
	TERM_RED, TERM_WHITE, TERM_RED, TERM_SLATE, TERM_WHITE,
	TERM_GREEN, TERM_L_GREEN, TERM_RED, TERM_L_WHITE, TERM_L_GREEN,
	TERM_UMBER, TERM_BLUE, TERM_GREEN, TERM_WHITE, TERM_L_WHITE,
	TERM_L_RED, TERM_L_WHITE, TERM_WHITE, TERM_L_WHITE, TERM_L_WHITE,
	TERM_L_RED, TERM_RED, TERM_BLUE, TERM_YELLOW, TERM_YELLOW,
	TERM_L_BLUE, TERM_L_UMBER, TERM_WHITE, TERM_L_UMBER, TERM_YELLOW,
	TERM_L_DARK, TERM_L_WHITE, TERM_UMBER, TERM_L_BLUE, TERM_L_DARK,
	TERM_YELLOW, TERM_L_GREEN
};


/*
 * Amulets (adjectives and colors)
 */

static cptr amulet_adj[MAX_AMULETS] =
{
	"Amber", "Driftwood", "Coral", "Agate", "Ivory",
	"Obsidian", "Bone", "Brass", "Bronze", "Pewter",
	"Tortoise Shell", "Golden", "Azure", "Crystal", "Silver",
    "Copper", "Emerald", "Ruby", "Amethyst", "Mithril",
    "Sapphire", "Dragon Tooth", "Sea Shell", "Flint Stone",
    "Platinum", "Glass"
};

static byte amulet_col[MAX_AMULETS] =
{
	TERM_YELLOW, TERM_L_UMBER, TERM_WHITE, TERM_L_WHITE, TERM_WHITE,
    TERM_L_DARK, TERM_WHITE, TERM_ORANGE, TERM_L_UMBER, TERM_SLATE,
	TERM_UMBER, TERM_YELLOW, TERM_L_BLUE, TERM_WHITE, TERM_L_WHITE,
    TERM_L_UMBER, TERM_GREEN, TERM_RED, TERM_VIOLET, TERM_L_BLUE,
    TERM_BLUE, TERM_L_WHITE, TERM_L_BLUE, TERM_SLATE,
    TERM_L_WHITE, TERM_WHITE
};


/*
 * Staffs (adjectives and colors)
 */

static cptr staff_adj[MAX_WOODS] =
{
	"Aspen", "Balsa", "Banyan", "Birch", "Cedar",
	"Cottonwood", "Cypress", "Dogwood", "Elm", "Eucalyptus",
	"Hemlock", "Hickory", "Ironwood", "Locust", "Mahogany",
	"Maple", "Mulberry", "Oak", "Pine", "Redwood",
	"Rosewood", "Spruce", "Sycamore", "Teak", "Walnut",
	"Mistletoe", "Hawthorn", "Bamboo", "Silver", "Runed",
	"Golden", "Ashen"/*,"Gnarled","Ivory","Willow"*/
};

static byte staff_col[MAX_WOODS] =
{
	TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER,
	TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER,
	TERM_L_UMBER, TERM_L_UMBER, TERM_UMBER, TERM_L_UMBER, TERM_UMBER,
	TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_RED,
	TERM_RED, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_UMBER,
	TERM_GREEN, TERM_L_UMBER, TERM_L_UMBER, TERM_L_WHITE, TERM_UMBER,
	TERM_YELLOW, TERM_SLATE, /*???,???,???*/
};


/*
 * Wands (adjectives and colors)
 */

static cptr wand_adj[MAX_METALS] =
{
    "Aluminium", "Cast Iron", "Chromium", "Copper", "Gold",
	"Iron", "Magnesium", "Molybdenum", "Nickel", "Rusty",
	"Silver", "Steel", "Tin", "Titanium", "Tungsten",
    "Zirconium", "Zinc", "Aluminium-Plated", "Copper-Plated", "Gold-Plated",
	"Nickel-Plated", "Silver-Plated", "Steel-Plated", "Tin-Plated", "Zinc-Plated",
	"Mithril-Plated", "Mithril", "Runed", "Bronze", "Brass",
	"Platinum", "Lead"/*,"Lead-Plated","Ivory","Pewter"*/
};

static byte wand_col[MAX_METALS] =
{
	TERM_L_BLUE, TERM_L_DARK, TERM_WHITE, TERM_L_UMBER, TERM_YELLOW,
	TERM_SLATE, TERM_L_WHITE, TERM_L_WHITE, TERM_L_UMBER, TERM_RED,
	TERM_L_WHITE, TERM_L_WHITE, TERM_L_WHITE, TERM_WHITE, TERM_WHITE,
	TERM_L_WHITE, TERM_L_WHITE, TERM_L_BLUE, TERM_L_UMBER, TERM_YELLOW,
	TERM_L_UMBER, TERM_L_WHITE, TERM_L_WHITE, TERM_L_WHITE, TERM_L_WHITE,
	TERM_L_BLUE, TERM_L_BLUE, TERM_UMBER, TERM_L_UMBER, TERM_L_UMBER,
	TERM_WHITE, TERM_SLATE, /*TERM_SLATE,TERM_WHITE,TERM_SLATE*/
};


/*
 * Rods (adjectives and colors).
 * Efficiency -- copied from wand arrays
 */

static cptr rod_adj[MAX_METALS];

static byte rod_col[MAX_METALS];


/*
 * Mushrooms (adjectives and colors)
 */

static cptr food_adj[MAX_SHROOM] =
{
	"Blue", "Black", "Black Spotted", "Brown", "Dark Blue",
	"Dark Green", "Dark Red", "Yellow", "Furry", "Green",
	"Grey", "Light Blue", "Light Green", "Violet", "Red",
	"Slimy", "Tan", "White", "White Spotted", "Wrinkled",
};

static byte food_col[MAX_SHROOM] =
{
	TERM_BLUE, TERM_L_DARK, TERM_L_DARK, TERM_UMBER, TERM_BLUE,
	TERM_GREEN, TERM_RED, TERM_YELLOW, TERM_L_WHITE, TERM_GREEN,
	TERM_SLATE, TERM_L_BLUE, TERM_L_GREEN, TERM_VIOLET, TERM_RED,
	TERM_SLATE, TERM_L_UMBER, TERM_WHITE, TERM_WHITE, TERM_UMBER
};


/*
 * Color adjectives and colors, for potions.
 * Hack -- The first four entries are hard-coded.
 * (water, apple juice, slime mold juice, something)
 */

static cptr potion_adj[MAX_COLORS] =
{
	"Clear", "Light Brown", "Icky Green", "xxx",
	"Azure", "Blue", "Blue Speckled", "Black", "Brown", "Brown Speckled",
	"Bubbling", "Chartreuse", "Cloudy", "Copper Speckled", "Crimson", "Cyan",
	"Dark Blue", "Dark Green", "Dark Red", "Gold Speckled", "Green",
	"Green Speckled", "Grey", "Grey Speckled", "Hazy", "Indigo",
	"Light Blue", "Light Green", "Magenta", "Metallic Blue", "Metallic Red",
	"Metallic Green", "Metallic Purple", "Misty", "Orange", "Orange Speckled",
	"Pink", "Pink Speckled", "Puce", "Purple", "Purple Speckled",
	"Red", "Red Speckled", "Silver Speckled", "Smoky", "Tangerine",
	"Violet", "Vermilion", "White", "Yellow", "Violet Speckled",
	"Pungent", "Clotted Red", "Viscous Pink", "Oily Yellow", "Gloopy Green",
	"Shimmering", "Coagulated Crimson", "Yellow Speckled", "Gold"
};

static byte potion_col[MAX_COLORS] =
{
	TERM_WHITE, TERM_L_UMBER, TERM_GREEN, 0,
	TERM_L_BLUE, TERM_BLUE, TERM_BLUE, TERM_L_DARK, TERM_UMBER, TERM_UMBER,
	TERM_L_WHITE, TERM_L_GREEN, TERM_WHITE, TERM_L_UMBER, TERM_RED, TERM_L_BLUE,
	TERM_BLUE, TERM_GREEN, TERM_RED, TERM_YELLOW, TERM_GREEN,
	TERM_GREEN, TERM_SLATE, TERM_SLATE, TERM_L_WHITE, TERM_VIOLET,
	TERM_L_BLUE, TERM_L_GREEN, TERM_RED, TERM_BLUE, TERM_RED,
	TERM_GREEN, TERM_VIOLET, TERM_L_WHITE, TERM_ORANGE, TERM_ORANGE,
	TERM_L_RED, TERM_L_RED, TERM_VIOLET, TERM_VIOLET, TERM_VIOLET,
	TERM_RED, TERM_RED, TERM_L_WHITE, TERM_L_DARK, TERM_ORANGE,
	TERM_VIOLET, TERM_RED, TERM_WHITE, TERM_YELLOW, TERM_VIOLET,
	TERM_L_RED, TERM_RED, TERM_L_RED, TERM_YELLOW, TERM_GREEN,
	TERM_MULTI, TERM_RED, TERM_YELLOW, TERM_YELLOW
};


/*
 * Syllables for scrolls (must be 1-4 letters each)
 */

static cptr syllables[MAX_SYLLABLES] =
{
	"a", "ab", "ag", "aks", "ala", "an", "ankh", "app",
	"arg", "arze", "ash", "aus", "ban", "bar", "bat", "bek",
	"bie", "bin", "bit", "bjor", "blu", "bot", "bu",
	"byt", "comp", "con", "cos", "cre", "dalf", "dan",
	"den", "der", "doe", "dok", "eep", "el", "eng", "er", "ere", "erk",
	"esh", "evs", "fa", "fid", "flit", "for", "fri", "fu", "gan",
	"gar", "glen", "gop", "gre", "ha", "he", "hyd", "i",
	"ing", "ion", "ip", "ish", "it", "ite", "iv", "jo",
	"kho", "kli", "klis", "la", "lech", "man", "mar",
	"me", "mi", "mic", "mik", "mon", "mung", "mur", "nag", "nej",
	"nelg", "nep", "ner", "nes", "nis", "nih", "nin", "o",
	"od", "ood", "org", "orn", "ox", "oxy", "pay", "pet",
	"ple", "plu", "po", "pot", "prok", "re", "rea", "rhov",
	"ri", "ro", "rog", "rok", "rol", "sa", "san", "sat",
	"see", "sef", "seh", "shu", "ski", "sna", "sne", "snik",
	"sno", "so", "sol", "sri", "sta", "sun", "ta", "tab",
	"tem", "ther", "ti", "tox", "trol", "tue", "turs", "u",
	"ulk", "um", "un", "uni", "ur", "val", "viv", "vly",
	"vom", "wah", "wed", "werg", "wex", "whon", "wun", "x",
	"yerg", "yp", "zun", "tri", "blaa"
};


/*
 * Hold the titles of scrolls, 6 to 14 characters each
 * Also keep an array of scroll colors (always WHITE for now)
 */

static char scroll_adj[MAX_TITLES][16];

static byte scroll_col[MAX_TITLES];






/*
 * Certain items have a flavor
 * This function is used only by "flavor_init()"
 */
static bool object_has_flavor(int i)
{
	object_kind *k_ptr = &k_info[i];

	/* Check for flavor */
	switch (k_ptr->tval)
	{
		/* The standard "flavored" items */
		case TV_AMULET:
		case TV_RING:
		case TV_STAFF:
		case TV_WAND:
		case TV_SCROLL:
		case TV_POTION:
		case TV_ROD:
		{
			return (TRUE);
		}

		/* Hack -- food SOMETIMES has a flavor */
		case TV_FOOD:
		{
			if (k_ptr->sval < SV_FOOD_MIN_FOOD) return (TRUE);
			return (FALSE);
		}
	}

	/* Assume no flavor */
	return (FALSE);
}


/*
 * Certain items, if aware, are known instantly
 * This function is used only by "flavor_init()"
 *
 * XXX XXX XXX Add "EASY_KNOW" flag to "k_info.txt" file
 */
static bool object_easy_know(int i)
{
	object_kind *k_ptr = &k_info[i];

	/* Analyze the "tval" */
	switch (k_ptr->tval)
	{
		/* Spellbooks */
		case TV_MAGIC_BOOK:
		case TV_PRAYER_BOOK:
		{
			return (TRUE);
		}

		/* Simple items */
		case TV_FLASK:
		case TV_JUNK:
		case TV_BOTTLE:
		case TV_SKELETON:
		case TV_SPIKE:
		{
			return (TRUE);
		}

		/* All Food, Potions, Scrolls, Rods */
		case TV_FOOD:
		case TV_POTION:
		case TV_SCROLL:
		case TV_ROD:
		{
			return (TRUE);
		}

		/* Some Rings, Amulets, Lites */
		case TV_RING:
		case TV_AMULET:
		case TV_LITE:
		{
			if (k_ptr->flags3 & TR3_EASY_KNOW) return (TRUE);
			return (FALSE);
		}
	}

	/* Nope */
	return (FALSE);
}


/*
 * Hack -- prepare the default object attr codes by tval
 *
 * XXX XXX XXX Off-load to "pref.prf" file
 */
static byte default_tval_to_attr(int tval)
{
	switch (tval)
	{
		case TV_SKELETON:
		case TV_BOTTLE:
		case TV_JUNK:
		{
			return (TERM_WHITE);
		}

		case TV_CHEST:
		{
			return (TERM_SLATE);
		}

		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		{
			return (TERM_L_UMBER);
		}

		case TV_LITE:
		{
			return (TERM_YELLOW);
		}

		case TV_SPIKE:
		{
			return (TERM_SLATE);
		}

		case TV_BOW:
		{
			return (TERM_UMBER);
		}

		case TV_DIGGING:
		{
			return (TERM_SLATE);
		}

		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		{
			return (TERM_L_WHITE);
		}

        case TV_MSTAFF:
        {
            return (TERM_L_BLUE);
        }

		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CROWN:
		case TV_HELM:
		case TV_SHIELD:
		case TV_CLOAK:
		{
			return (TERM_L_UMBER);
		}

		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		{
			return (TERM_SLATE);
		}

		case TV_AMULET:
		{
			return (TERM_ORANGE);
		}

		case TV_RING:
		{
			return (TERM_RED);
		}

		case TV_STAFF:
		{
			return (TERM_L_UMBER);
		}

		case TV_WAND:
		{
			return (TERM_L_GREEN);
		}

		case TV_ROD:
		{
			return (TERM_L_WHITE);
		}

		case TV_SCROLL:
		{
			return (TERM_WHITE);
		}

		case TV_POTION:
		{
			return (TERM_L_BLUE);
		}

		case TV_FLASK:
		{
			return (TERM_YELLOW);
		}

		case TV_FOOD:
		{
			return (TERM_L_UMBER);
		}

		case TV_MAGIC_BOOK:
		{
			return (TERM_L_RED);
		}

		case TV_PRAYER_BOOK:
		{
			return (TERM_L_GREEN);
		}
	}

	return (TERM_WHITE);
}


/*
 * Hack -- prepare the default object char codes by tval
 *
 * XXX XXX XXX Off-load to "pref.prf" file (?)
 */
static byte default_tval_to_char(int tval)
{
	int i;

	/* Hack -- Guess at "correct" values for tval_to_char[] */
	for (i = 1; i < MAX_K_IDX; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Use the first value we find */
		if (k_ptr->tval == tval) return (k_ptr->k_char);
	}

	/* Default to space */
	return (' ');
}



/*
 * Prepare the "variable" part of the "k_info" array.
 *
 * The "color"/"metal"/"type" of an item is its "flavor".
 * For the most part, flavors are assigned randomly each game.
 *
 * Initialize descriptions for the "colored" objects, including:
 * Rings, Amulets, Staffs, Wands, Rods, Food, Potions, Scrolls.
 *
 * The first 4 entries for potions are fixed (Water, Apple Juice,
 * Slime Mold Juice, Unused Potion).
 *
 * Scroll titles are always between 6 and 14 letters long.  This is
 * ensured because every title is composed of whole words, where every
 * word is from 1 to 8 letters long (one or two syllables of 1 to 4
 * letters each), and that no scroll is finished until it attempts to
 * grow beyond 15 letters.  The first time this can happen is when the
 * current title has 6 letters and the new word has 8 letters, which
 * would result in a 6 letter scroll title.
 *
 * Duplicate titles are avoided by requiring that no two scrolls share
 * the same first four letters (not the most efficient method, and not
 * the least efficient method, but it will always work).
 *
 * Hack -- make sure everything stays the same for each saved game
 * This is accomplished by the use of a saved "random seed", as in
 * "town_gen()".  Since no other functions are called while the special
 * seed is in effect, so this function is pretty "safe".
 *
 * Note that the "hacked seed" may provide an RNG with alternating parity!
 */
void flavor_init(void)
{
	int		i, j;

	byte	temp_col;

	cptr	temp_adj;


	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant flavors */
	Rand_value = seed_flavor;


	/* Efficiency -- Rods/Wands share initial array */
	for (i = 0; i < MAX_METALS; i++)
	{
		rod_adj[i] = wand_adj[i];
		rod_col[i] = wand_col[i];
	}


	/* Rings have "ring colors" */
	for (i = 0; i < MAX_ROCKS; i++)
	{
		j = rand_int(MAX_ROCKS);
		temp_adj = ring_adj[i];
		ring_adj[i] = ring_adj[j];
		ring_adj[j] = temp_adj;
		temp_col = ring_col[i];
		ring_col[i] = ring_col[j];
		ring_col[j] = temp_col;
	}

	/* Amulets have "amulet colors" */
	for (i = 0; i < MAX_AMULETS; i++)
	{
		j = rand_int(MAX_AMULETS);
		temp_adj = amulet_adj[i];
		amulet_adj[i] = amulet_adj[j];
		amulet_adj[j] = temp_adj;
		temp_col = amulet_col[i];
		amulet_col[i] = amulet_col[j];
		amulet_col[j] = temp_col;
	}

	/* Staffs */
	for (i = 0; i < MAX_WOODS; i++)
	{
		j = rand_int(MAX_WOODS);
		temp_adj = staff_adj[i];
		staff_adj[i] = staff_adj[j];
		staff_adj[j] = temp_adj;
		temp_col = staff_col[i];
		staff_col[i] = staff_col[j];
		staff_col[j] = temp_col;
	}

	/* Wands */
	for (i = 0; i < MAX_METALS; i++)
	{
		j = rand_int(MAX_METALS);
		temp_adj = wand_adj[i];
		wand_adj[i] = wand_adj[j];
		wand_adj[j] = temp_adj;
		temp_col = wand_col[i];
		wand_col[i] = wand_col[j];
		wand_col[j] = temp_col;
	}

	/* Rods */
	for (i = 0; i < MAX_METALS; i++)
	{
		j = rand_int(MAX_METALS);
		temp_adj = rod_adj[i];
		rod_adj[i] = rod_adj[j];
		rod_adj[j] = temp_adj;
		temp_col = rod_col[i];
		rod_col[i] = rod_col[j];
		rod_col[j] = temp_col;
	}

	/* Foods (Mushrooms) */
	for (i = 0; i < MAX_SHROOM; i++)
	{
		j = rand_int(MAX_SHROOM);
		temp_adj = food_adj[i];
		food_adj[i] = food_adj[j];
		food_adj[j] = temp_adj;
		temp_col = food_col[i];
		food_col[i] = food_col[j];
		food_col[j] = temp_col;
	}

	/* Potions */
	for (i = 4; i < MAX_COLORS; i++)
	{
		j = rand_int(MAX_COLORS - 4) + 4;
		temp_adj = potion_adj[i];
		potion_adj[i] = potion_adj[j];
		potion_adj[j] = temp_adj;
		temp_col = potion_col[i];
		potion_col[i] = potion_col[j];
		potion_col[j] = temp_col;
	}

	/* Scrolls (random titles, always white) */
	for (i = 0; i < MAX_TITLES; i++)
	{
		/* Get a new title */
		while (TRUE)
		{
			char buf[80];

			bool okay;

			/* Start a new title */
			buf[0] = '\0';

			/* Collect words until done */
			while (1)
			{
				int q, s;

				char tmp[80];

				/* Start a new word */
				tmp[0] = '\0';

				/* Choose one or two syllables */
				s = ((rand_int(100) < 30) ? 1 : 2);

				/* Add a one or two syllable word */
				for (q = 0; q < s; q++)
				{
					/* Add the syllable */
					strcat(tmp, syllables[rand_int(MAX_SYLLABLES)]);
				}

				/* Stop before getting too long */
				if (strlen(buf) + 1 + strlen(tmp) > 15) break;

				/* Add a space */
				strcat(buf, " ");

				/* Add the word */
				strcat(buf, tmp);
			}

			/* Save the title */
			strcpy(scroll_adj[i], buf+1);

			/* Assume okay */
			okay = TRUE;

			/* Check for "duplicate" scroll titles */
			for (j = 0; j < i; j++)
			{
				cptr hack1 = scroll_adj[j];
				cptr hack2 = scroll_adj[i];

				/* Compare first four characters */
				if (*hack1++ != *hack2++) continue;
				if (*hack1++ != *hack2++) continue;
				if (*hack1++ != *hack2++) continue;
				if (*hack1++ != *hack2++) continue;

				/* Not okay */
				okay = FALSE;

				/* Stop looking */
				break;
			}

			/* Break when done */
			if (okay) break;
		}

		/* All scrolls are white */
		scroll_col[i] = TERM_WHITE;
	}


	/* Hack -- Use the "complex" RNG */
	Rand_quick = FALSE;

	/* Analyze every object */
	for (i = 1; i < MAX_K_IDX; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Skip "empty" objects */
		if (!k_ptr->name) continue;

		/* Check for a "flavor" */
		k_ptr->has_flavor = object_has_flavor(i);

		/* No flavor yields aware */
		/*if (!k_ptr->has_flavor) k_ptr->aware = TRUE;*/

		/* Check for "easily known" */
		k_ptr->easy_know = object_easy_know(i);
	}
}




/*
 * Extract the "default" attr for each object
 * This function is used only by "flavor_init()"
 */
static byte object_d_attr(int i)
{
	object_kind *k_ptr = &k_info[i];

	/* Flavored items */
	if (k_ptr->has_flavor)
	{
		/* Extract the indexx */
		int indexx = k_ptr->sval;

		/* Analyze the item */
		switch (k_ptr->tval)
		{
			case TV_FOOD:   return (food_col[indexx]);
			case TV_POTION: return (potion_col[indexx]);
			case TV_SCROLL: return (scroll_col[indexx]);
			case TV_AMULET: return (amulet_col[indexx]);
			case TV_RING:   return (ring_col[indexx]);
			case TV_STAFF:  return (staff_col[indexx]);
			case TV_WAND:   return (wand_col[indexx]);
			case TV_ROD:    return (rod_col[indexx]);
		}
	}

	/* Default attr if legal */
	if (k_ptr->k_attr) return (k_ptr->k_attr);

	/* Default to white */
	return (TERM_WHITE);
}


/*
 * Extract the "default" char for each object
 * This function is used only by "flavor_init()"
 */
static byte object_d_char(int i)
{
	object_kind *k_ptr = &k_info[i];

	return (k_ptr->k_char);
}


/*
 * Reset the "visual" lists
 *
 * This is useful for switching on/off the "use_graphics" flag.
 */
void reset_visuals(void)
{
	int i;

	/* Extract some info about terrain features */
	for (i = 0; i < MAX_F_IDX; i++)
	{
		feature_type *f_ptr = &f_info[i];

		/* Assume we will use the underlying values */
		f_ptr->z_attr = f_ptr->f_attr;
		f_ptr->z_char = f_ptr->f_char;
	}

	/* Extract some info about objects */
	for (i = 0; i < MAX_K_IDX; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Extract the "underlying" attr */
		k_ptr->d_attr = object_d_attr(i);

		/* Extract the "underlying" char */
		k_ptr->d_char = object_d_char(i);

		/* Assume we will use the underlying values */
		k_ptr->x_attr = k_ptr->d_attr;
		k_ptr->x_char = k_ptr->d_char;
	}

	/* Extract some info about monsters */
	for (i = 0; i < MAX_R_IDX; i++)
	{
		/* Extract the "underlying" attr */
		r_info[i].x_attr = r_info[i].d_attr;

		/* Extract the "underlying" char */
		r_info[i].x_char = r_info[i].d_char;
	}

	/* Extract attr/chars for equippy items (by tval) */
	for (i = 0; i < 128; i++)
	{
		/* Extract a default attr */
		tval_to_attr[i] = default_tval_to_attr(i);

		/* Extract a default char */
		tval_to_char[i] = default_tval_to_char(i);
	}
}









/*
 * Obtain the "flags" for an item
 */
void object_flags(object_type *o_ptr, u32b *f1, u32b *f2, u32b *f3, u32b *f4)
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/* Base object */
	(*f1) = k_ptr->flags1;
	(*f2) = k_ptr->flags2;
	(*f3) = k_ptr->flags3;
    (*f4) = k_ptr->flags4;

	/* Artifact */
    if (artifact_p(o_ptr))
	{
        artifact_type *a_ptr;
#if 0
	if (o_ptr->name1 == ART_RANDART)
	{
		a_ptr = randart_make(o_ptr);
	}
	else
	{
#endif
		a_ptr = &a_info[o_ptr->name1];
#if 0
	}
#endif
	if (a_ptr == NULL) return;

		(*f1) = a_ptr->flags1;
		(*f2) = a_ptr->flags2;
		(*f3) = a_ptr->flags3;
	(*f4) = a_ptr->flags4;
	}

	/* Ego-item */
	if (o_ptr->name2)
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		(*f1) |= e_ptr->flags1;
		(*f2) |= e_ptr->flags2;
		(*f3) |= e_ptr->flags3;
	(*f4) |= e_ptr->flags4;
	}

	/* Extra powers */
	switch (o_ptr->xtra1)
	{
		case EGO_XTRA_SUSTAIN:
		{
			/* Choose a sustain */
			switch (o_ptr->xtra2 % 6)
			{
				case 0: (*f2) |= TR2_SUST_STR; break;
				case 1: (*f2) |= TR2_SUST_INT; break;
				case 2: (*f2) |= TR2_SUST_WIS; break;
				case 3: (*f2) |= TR2_SUST_DEX; break;
				case 4: (*f2) |= TR2_SUST_CON; break;
				case 5: (*f2) |= TR2_SUST_CHR; break;
			}

			break;
		}

		case EGO_XTRA_POWER:
		{
			/* Choose a power */
            switch (o_ptr->xtra2 % 11)
			{
				case 0: (*f2) |= TR2_RES_BLIND; break;
				case 1: (*f2) |= TR2_RES_CONF; break;
				case 2: (*f2) |= TR2_RES_SOUND; break;
				case 3: (*f2) |= TR2_RES_SHARDS; break;
				case 4: (*f2) |= TR2_RES_NETHER; break;
				case 5: (*f2) |= TR2_RES_NEXUS; break;
				case 6: (*f2) |= TR2_RES_CHAOS; break;
				case 7: (*f2) |= TR2_RES_DISEN; break;
				case 8: (*f2) |= TR2_RES_POIS; break;
		case 9: (*f2) |= TR2_RES_LITE; break; /* was missing... why??? */
		case 10: (*f2) |= TR2_RES_DARK; break; /* was missing... why??? */
			}

			break;
		}

		case EGO_XTRA_ABILITY:
		{
			/* Choose an ability */
            switch (o_ptr->xtra2 % 9)
			{
				case 0: (*f3) |= TR3_FEATHER; break;
				case 1: (*f3) |= TR3_LITE; break;
				case 2: (*f3) |= TR3_SEE_INVIS; break;
                case 3: (*f4) |= TR4_ESP_ALL; break;
				case 4: (*f3) |= TR3_SLOW_DIGEST; break;
				case 5: (*f3) |= TR3_REGEN; break;
				case 6: (*f2) |= TR2_FREE_ACT; break;
				case 7: (*f2) |= TR2_HOLD_LIFE; break;
		case 8: (*f2) |= TR2_RES_FEAR; break;
			}

			break;
		}
	}
}







/*
 * Print a char "c" into a string "t", as if by sprintf(t, "%c", c),
 * and return a pointer to the terminator (t + 1).
 */
static char *object_desc_chr(char *t, char c)
{
	/* Copy the char */
	*t++ = c;

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}


/*
 * Print a string "s" into a string "t", as if by strcpy(t, s),
 * and return a pointer to the terminator.
 */
static char *object_desc_str(char *t, cptr s)
{
	/* Copy the string */
	while (*s) *t++ = *s++;

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}



/*
 * Print an unsigned number "n" into a string "t", as if by
 * sprintf(t, "%u", n), and return a pointer to the terminator.
 */
static char *object_desc_num(char *t, uint n)
{
	uint p;

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}




/*
 * Print an signed number "v" into a string "t", as if by
 * sprintf(t, "%+d", n), and return a pointer to the terminator.
 * Note that we always print a sign, either "+" or "-".
 */
static char *object_desc_int(char *t, sint v)
{
	uint p, n;

	/* Negative */
	if (v < 0)
	{
		/* Take the absolute value */
		n = 0 - v;

		/* Use a "minus" sign */
		*t++ = '-';
	}

	/* Positive (or zero) */
	else
	{
		/* Use the actual number */
		n = v;

		/* Use a "plus" sign */
		*t++ = '+';
	}

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}



/*
 * Creates a description of the item "o_ptr", and stores it in "out_val".
 *
 * One can choose the "verbosity" of the description, including whether
 * or not the "number" of items should be described, and how much detail
 * should be used when describing the item.
 *
 * The given "buf" must be 80 chars long to hold the longest possible
 * description, which can get pretty long, including incriptions, such as:
 * "no more Maces of Disruption (Defender) (+10,+10) [+5] (+3 to stealth)".
 * Note that the inscription will be clipped to keep the total description
 * under 79 chars (plus a terminator).
 *
 * Note the use of "object_desc_num()" and "object_desc_int()" as hyper-efficient,
 * portable, versions of some common "sprintf()" commands.
 *
 * Note that all ego-items (when known) append an "Ego-Item Name", unless
 * the item is also an artifact, which should NEVER happen.
 *
 * Note that all artifacts (when known) append an "Artifact Name", so we
 * have special processing for "Specials" (artifact Lites, Rings, Amulets).
 * The "Specials" never use "modifiers" if they are "known", since they
 * have special "descriptions", such as "The Necklace of the Dwarves".
 *
 * Special Lite's use the "k_info" base-name (Phial, Star, or Arkenstone),
 * plus the artifact name, just like any other artifact, if known.
 *
 * Special Ring's and Amulet's, if not "aware", use the same code as normal
 * rings and amulets, and if "aware", use the "k_info" base-name (Ring or
 * Amulet or Necklace).  They will NEVER "append" the "k_info" name.  But,
 * they will append the artifact name, just like any artifact, if known.
 *
 * None of the Special Rings/Amulets are "EASY_KNOW", though they could be,
 * at least, those which have no "pluses", such as the three artifact lites.
 *
 * Hack -- Display "The One Ring" as "a Plain Gold Ring" until aware.
 *
 * If "pref" then a "numeric" prefix will be pre-pended.
 *
 * Mode:
 *   0 -- The Cloak of Death
 *   1 -- The Cloak of Death [1,+3]
 *   2 -- The Cloak of Death [1,+3] (+2 to Stealth)
 *   3 -- The Cloak of Death [1,+3] (+2 to Stealth) {nifty}
 */
void object_desc(int Ind, char *buf, object_type *o_ptr, int pref, int mode)
{
	cptr		basenm, modstr;
	int			power, indexx;

	bool		aware = FALSE;
	bool		known = FALSE;

	bool		append_name = FALSE;

	bool		show_weapon = FALSE;
	bool		show_armour = FALSE;

	cptr		s, u;
	char		*t;

	char		p1 = '(', p2 = ')';
	char		b1 = '[', b2 = ']';
	char		c1 = '{', c2 = '}';

	char		tmp_val[160];

    u32b		f1, f2, f3, f4;

	object_kind		*k_ptr = &k_info[o_ptr->k_idx];


	/* Extract some flags */
    object_flags(o_ptr, &f1, &f2, &f3, &f4);

	/* Assume aware and known if not a valid player */
	if (Ind)
	{
		/* See if the object is "aware" */
		if (object_aware_p(Ind, o_ptr)) aware = TRUE;

		/* See if the object is "known" */
		if (object_known_p(Ind, o_ptr)) known = TRUE;
	}
	else
	{
		/* Assume aware and known */
		aware = known = TRUE;
	}

	/* Hack -- Extract the sub-type "indexx" */
	indexx = o_ptr->sval;

	/* Extract default "base" string */
	basenm = (k_name + k_ptr->name);

	/* Assume no "modifier" string */
	modstr = "";

	/* Analyze the object */
	switch (o_ptr->tval)
	{
			/* Some objects are easy to describe */
		case TV_SKELETON:
		case TV_BOTTLE:
		case TV_JUNK:
		case TV_SPIKE:
		case TV_KEY:
		case TV_FLASK:
		case TV_CHEST:
		{
			break;
		}

			/* Missiles/ Bows/ Weapons */
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		case TV_BOW:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
        case TV_MSTAFF:
		case TV_DIGGING:
		{
			show_weapon = TRUE;
			break;
		}

			/* Armour */
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_SHIELD:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		{
			show_armour = TRUE;
			break;
		}

			/* Lites (including a few "Specials") */
		case TV_LITE:
		{
			break;
		}

			/* Amulets (including a few "Specials") */
		case TV_AMULET:
		{
			/* Known artifacts */
	    /* if (artifact_p(o_ptr) && known) break;*/

			/* Color the object */
			modstr = amulet_adj[indexx];
            if (aware && !true_artifact_p(o_ptr)) append_name = TRUE;
            if (!aware) basenm = "& # Amulet~";
            else if (!true_artifact_p(o_ptr)) basenm = "& Amulet~";
			break;
		}


			/* Rings (including a few "Specials") */
		case TV_RING:
		{
			/* Known artifacts */
	    /*if (artifact_p(o_ptr) && known) break;*/

			/* Color the object */
			modstr = ring_adj[indexx];
            if (aware && !true_artifact_p(o_ptr)) append_name = TRUE;
            if (!aware) basenm = "& # Ring~";
            else if (!true_artifact_p(o_ptr)) basenm = "& Ring~";

			/* Hack -- The One Ring */
			if (!aware && (o_ptr->sval == SV_RING_POWER)) modstr = "Plain Gold";

			break;
		}


		case TV_STAFF:
		{
			/* Color the object */
			modstr = staff_adj[indexx];
			if (aware) append_name = TRUE;
			basenm = aware ? "& Staff~" : "& # Staff~";
			break;
		}

		case TV_WAND:
		{
			/* Color the object */
			modstr = wand_adj[indexx];
			if (aware) append_name = TRUE;
			basenm = aware ? "& Wand~" : "& # Wand~";
			break;
		}

		case TV_ROD:
		{
			/* Color the object */
			modstr = rod_adj[indexx];
			if (aware) append_name = TRUE;
			basenm = aware ? "& Rod~" : "& # Rod~";
			break;
		}

		case TV_SCROLL:
		{
			/* Color the object */
			modstr = scroll_adj[indexx];
			if (aware) append_name = TRUE;
			basenm = aware ? "& Scroll~" : "& Scroll~ titled \"#\"";
			break;
		}

		case TV_POTION:
		{
			/* Color the object */
			modstr = potion_adj[indexx];
			if (aware) append_name = TRUE;
			basenm = aware ? "& Potion~" : "& # Potion~";
			break;
		}

		case TV_FOOD:
		{
			/* Ordinary food is "boring" */
			if (o_ptr->sval >= SV_FOOD_MIN_FOOD) break;

			/* Color the object */
			modstr = food_adj[indexx];
			if (aware) append_name = TRUE;
			basenm = aware ? "& Mushroom~" : "& # Mushroom~";
			break;
		}


			/* Magic Books */
		case TV_MAGIC_BOOK:
		{
			modstr = basenm;
			basenm = "& Book~ of Magic Spells #";
			break;
		}

			/* Prayer Books */
		case TV_PRAYER_BOOK:
		{
			modstr = basenm;
			basenm = "& Holy Book~ of Prayers #";
			break;
		}

			/* Hack -- Gold/Gems */
		case TV_GOLD:
		{
			strcpy(buf, basenm);
			return;
		}

			/* Used in the "inventory" routine */
		default:
		{
			strcpy(buf, "(nothing)");
			return;
		}
	}


	/* Start dumping the result */
	t = buf;

	/* The object "expects" a "number" */
	if (basenm[0] == '&')
	{
		/* Skip the ampersand (and space) */
		s = basenm + 2;

		/* No prefix */
		if (!pref)
		{
			/* Nothing */
		}

		/* Hack -- None left */
		else if (o_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Extract the number */
		else if (o_ptr->number > 1)
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if (known && artifact_p(o_ptr))
		{
			t = object_desc_str(t, "The ");
		}

		/* A single one, with a vowel in the modifier */
		else if ((*s == '#') && (is_a_vowel(modstr[0])))
		{
			t = object_desc_str(t, "an ");
		}

		/* A single one, with a vowel */
		else if (is_a_vowel(*s))
		{
			t = object_desc_str(t, "an ");
		}

		/* A single one, without a vowel */
		else
		{
			t = object_desc_str(t, "a ");
		}
	}

	/* Hack -- objects that "never" take an article */
	else
	{
		/* No ampersand */
		s = basenm;

		/* No pref */
		if (!pref)
		{
			/* Nothing */
		}

		/* Hack -- all gone */
		else if (o_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Prefix a number if required */
		else if (o_ptr->number > 1)
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if (known && artifact_p(o_ptr))
		{
			t = object_desc_str(t, "The ");
		}

		/* Hack -- single items get no prefix */
		else
		{
			/* Nothing */
		}
	}

	/* Paranoia -- skip illegal tildes */
	/* while (*s == '~') s++; */

	/* Copy the string */
	for (; *s; s++)
	{
		/* Pluralizer */
		if (*s == '~')
		{
			/* Add a plural if needed */
			if (o_ptr->number != 1)
			{
				char k = t[-1];

				/* XXX XXX XXX Mega-Hack */

				/* Hack -- "Cutlass-es" and "Torch-es" */
				if ((k == 's') || (k == 'h')) *t++ = 'e';

				/* Add an 's' */
				*t++ = 's';
			}
		}

		/* Modifier */
		else if (*s == '#')
		{
			/* Insert the modifier */
			for (u = modstr; *u; u++) *t++ = *u;
		}

		/* Normal */
		else
		{
			/* Copy */
			*t++ = *s;
		}
	}

	/* Terminate */
	*t = '\0';


	/* Append the "kind name" to the "base name" */
	if (append_name)
	{
		t = object_desc_str(t, " of ");
		t = object_desc_str(t, (k_name + k_ptr->name));
	}


	/* Hack -- Append "Artifact" or "Special" names */
	if (known)
	{
        /* Grab any randart name */
#if defined(RANDARTS)
	if (o_ptr->name1 == ART_RANDART)
	{
		/* Create the name */
		randart_name(o_ptr, tmp_val);
			
		t = object_desc_chr(t, ' ');
		t = object_desc_str(t, tmp_val);
	}
			
		/* Grab any artifact name */
	else 
#endif
	if (artifact_p(o_ptr))
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];

			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, (a_name + a_ptr->name));
		}

		/* Grab any ego-item name */
		else if (o_ptr->name2)
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];

			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, (e_name + e_ptr->name));
		}
	}


	/* No more details wanted */
	if (mode < 1) return;


	/* Hack -- Chests must be described in detail */
	if (o_ptr->tval == TV_CHEST)
	{
		/* Not searched yet */
		if (!known)
		{
			/* Nothing */
		}

		/* May be "empty" */
		else if (!o_ptr->pval)
		{
			t = object_desc_str(t, " (empty)");
		}

		/* May be "disarmed" */
		else if (o_ptr->pval < 0)
		{
			if (chest_traps[o_ptr->pval])
			{
				t = object_desc_str(t, " (disarmed)");
			}
			else
			{
				t = object_desc_str(t, " (unlocked)");
			}
		}

		/* Describe the traps, if any */
		else
		{
			/* Describe the traps */
			switch (chest_traps[o_ptr->pval])
			{
				case 0:
				{
					t = object_desc_str(t, " (Locked)");
					break;
				}
				case CHEST_LOSE_STR:
				{
					t = object_desc_str(t, " (Poison Needle)");
					break;
				}
				case CHEST_LOSE_CON:
				{
					t = object_desc_str(t, " (Poison Needle)");
					break;
				}
				case CHEST_POISON:
				{
					t = object_desc_str(t, " (Gas Trap)");
					break;
				}
				case CHEST_PARALYZE:
				{
					t = object_desc_str(t, " (Gas Trap)");
					break;
				}
				case CHEST_EXPLODE:
				{
					t = object_desc_str(t, " (Explosion Device)");
					break;
				}
				case CHEST_SUMMON:
				{
					t = object_desc_str(t, " (Summoning Runes)");
					break;
				}
				default:
				{
					t = object_desc_str(t, " (Multiple Traps)");
					break;
				}
			}
		}
	}


	/* Display the item like a weapon */
	if (f3 & TR3_SHOW_MODS) show_weapon = TRUE;

	/* Display the item like a weapon */
	if (o_ptr->to_h && o_ptr->to_d) show_weapon = TRUE;

	/* Display the item like armour */
	if (o_ptr->ac) show_armour = TRUE;


	/* Dump base weapon info */
	switch (o_ptr->tval)
	{
		/* Missiles and Weapons */
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
        case TV_MSTAFF:
		case TV_DIGGING:

		/* Append a "damage" string */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, o_ptr->dd);
		t = object_desc_chr(t, 'd');
		t = object_desc_num(t, o_ptr->ds);
		t = object_desc_chr(t, p2);

		/* All done */
		break;


		/* Bows get a special "damage string" */
		case TV_BOW:

		/* Mega-Hack -- Extract the "base power" */
		power = (o_ptr->sval % 10);

		/* Apply the "Extra Might" flag */
		if (f3 & TR3_XTRA_MIGHT) power++;

		/* Append a special "damage" string */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_chr(t, 'x');
		t = object_desc_num(t, power);
		t = object_desc_chr(t, p2);

		/* All done */
		break;
	}


	/* Add the weapon bonuses */
	if (known)
	{
		/* Show the tohit/todam on request */
		if (show_weapon)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_h);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, o_ptr->to_d);
			t = object_desc_chr(t, p2);
		}

		/* Show the tohit if needed */
		else if (o_ptr->to_h)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_h);
			t = object_desc_chr(t, p2);
		}

		/* Show the todam if needed */
		else if (o_ptr->to_d)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_d);
			t = object_desc_chr(t, p2);
		}
	}


	/* Add the armor bonuses */
	if (known)
	{
		/* Show the armor class info */
		if (show_armour)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_num(t, o_ptr->ac);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, o_ptr->to_a);
			t = object_desc_chr(t, b2);
		}

		/* No base armor, but does increase armor */
		else if (o_ptr->to_a)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_int(t, o_ptr->to_a);
			t = object_desc_chr(t, b2);
		}
	}

	/* Hack -- always show base armor */
	else if (show_armour)
	{
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, b1);
		t = object_desc_num(t, o_ptr->ac);
		t = object_desc_chr(t, b2);
	}


	/* No more details wanted */
	if (mode < 2) return;


	/* Hack -- Wands and Staffs have charges */
	if (known &&
	    ((o_ptr->tval == TV_STAFF) ||
	     (o_ptr->tval == TV_WAND)))
	{
		/* Dump " (N charges)" */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, o_ptr->pval);
		t = object_desc_str(t, " charge");
		if (o_ptr->pval != 1) t = object_desc_chr(t, 's');
		t = object_desc_chr(t, p2);
	}

	/* Hack -- Rods have a "charging" indicator */
	else if (known && (o_ptr->tval == TV_ROD))
	{
		/* Hack -- Dump " (charging)" if relevant */
		if (o_ptr->pval) t = object_desc_str(t, " (charging)");
	}

	/* Hack -- Process Lanterns/Torches */
    else if ((o_ptr->tval == TV_LITE) && (o_ptr->sval < SV_LITE_DWARVEN) && (!o_ptr->name3))
	{
		/* Hack -- Turns of light for normal lites */
		t = object_desc_str(t, " (with ");
		t = object_desc_num(t, o_ptr->pval);
		t = object_desc_str(t, " turns of light)");
	}

	/* Dump "pval" flags for wearable items */
	if (known && (f1 & TR1_PVAL_MASK))
	{
		/* Hack -- first display any base pval bonuses.  
		 * The "bpval" flags are never displayed.  */
		if (o_ptr->bpval)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			/* Dump the "pval" itself */
			t = object_desc_int(t, o_ptr->bpval);
			t = object_desc_chr(t, p2);
		}
		/* Next, display any pval bonuses. */
		if (o_ptr->pval)
		{
			/* Start the display */
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);

			/* Dump the "pval" itself */
			t = object_desc_int(t, o_ptr->pval);

			/* Do not display the "pval" flags */
			if (f3 & TR3_HIDE_TYPE)
			{
				/* Nothing */
			}

			/* Speed */
			else if (f1 & TR1_SPEED)
			{
				/* Dump " to speed" */
				t = object_desc_str(t, " to speed");
			}

			/* Attack speed */
			else if (f1 & TR1_BLOWS)
			{
				/* Add " attack" */
				t = object_desc_str(t, " attack");

				/* Add "attacks" */
				if (ABS(o_ptr->pval) != 1) t = object_desc_chr(t, 's');
			}

			/* Stealth */
			else if (f1 & TR1_STEALTH)
			{
				/* Dump " to stealth" */
				t = object_desc_str(t, " to stealth");
			}

			/* Search */
			else if (f1 & TR1_SEARCH)
			{
				/* Dump " to searching" */
				t = object_desc_str(t, " to searching");
			}

			/* Infravision */
			else if (f1 & TR1_INFRA)
			{
				/* Dump " to infravision" */
				t = object_desc_str(t, " to infravision");
			}

			/* Tunneling */
			else if (f1 & TR1_TUNNEL)
			{
				/* Nothing */
			}

			/* Finish the display */
			t = object_desc_chr(t, p2);
		}
	}


	/* Indicate "charging" artifacts XXX XXX XXX */
	if (known && o_ptr->timeout)
	{
		/* Hack -- Dump " (charging)" if relevant */
		t = object_desc_str(t, " (charging)");
	}


	/* No more details wanted */
	if (mode < 3) return;


	/* No inscription yet */
	tmp_val[0] = '\0';

	/* Use the standard inscription if available */
	if (o_ptr->note)
	{
		strcpy(tmp_val, quark_str(o_ptr->note));
	}

	/* Note "cursed" if the item is known to be cursed */
	else if (cursed_p(o_ptr) && (known || (o_ptr->ident & ID_SENSE)))
	{
		strcpy(tmp_val, "cursed");
	}

	/* Mega-Hack -- note empty wands/staffs */
	else if (!known && (o_ptr->ident & ID_EMPTY))
	{
		strcpy(tmp_val, "empty");
	}

	/* Note "tried" if the object has been tested unsuccessfully */
	else if (!aware && object_tried_p(Ind, o_ptr))
	{
		strcpy(tmp_val, "tried");
	}

	/* Note the discount, if any */
	else if (o_ptr->discount)
	{
		object_desc_num(tmp_val, o_ptr->discount);
		strcat(tmp_val, "% off");
	}

	/* Append the inscription, if any */
	if (tmp_val[0])
	{
		int n;

		/* Hack -- How much so far */
		n = (t - buf);

		/* Paranoia -- do not be stupid */
		if (n > 75) n = 75;

		/* Hack -- shrink the inscription */
		tmp_val[75 - n] = '\0';

		/* Append the inscription */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, c1);
		t = object_desc_str(t, tmp_val);
		t = object_desc_chr(t, c2);
	}
}


/*
 * Hack -- describe an item currently in a store's inventory
 * This allows an item to *look* like the player is "aware" of it
 */
void object_desc_store(int Ind, char *buf, object_type *o_ptr, int pref, int mode)
{
	player_type *p_ptr = Players[Ind];

	bool hack_aware = FALSE;
	bool hack_known = FALSE;

	/* Only save flags if we have a valid player */
	if (Ind)
	{
		/* Save the "aware" flag */
		hack_aware = p_ptr->obj_aware[o_ptr->k_idx];

		/* Save the "known" flag */
		hack_known = (o_ptr->ident & ID_KNOWN) ? TRUE : FALSE;
	}

	/* Set the "known" flag */
	o_ptr->ident |= ID_KNOWN;

	/* Valid players only */
	if (Ind)
	{
		/* Force "aware" for description */
		p_ptr->obj_aware[o_ptr->k_idx] = TRUE;
	}


	/* Describe the object */
	object_desc(Ind, buf, o_ptr, pref, mode);


	/* Only restore flags if we have a valid player */
	if (Ind)
	{
		/* Restore "aware" flag */
		p_ptr->obj_aware[o_ptr->k_idx] = hack_aware;

		/* Clear the known flag */
		if (!hack_known) o_ptr->ident &= ~ID_KNOWN;
	}
}




/*
 * Determine the "Activation" (if any) for an artifact
 * Return a string, or NULL for "no activation"
 */
cptr item_activation(object_type *o_ptr)
{
    u32b f1, f2, f3, f4;

	/* Extract the flags */
    object_flags(o_ptr, &f1, &f2, &f3, &f4);

	/* Require activation ability */
	if (!(f3 & TR3_ACTIVATE)) return (NULL);

	/* Some artifacts can be activated */
	switch (o_ptr->name1)
	{
		case ART_NARTHANC:
		{
			return "fire bolt (9d8) every 8+d8 turns";
		}
		case ART_NIMTHANC:
		{
			return "frost bolt (6d8) every 7+d7 turns";
		}
		case ART_DETHANC:
		{
			return "lightning bolt (4d8) every 6+d6 turns";
		}
		case ART_RILIA:
		{
			return "stinking cloud (12) every 4+d4 turns";
		}
		case ART_BELANGIL:
		{
			return "frost ball (48) every 5+d5 turns";
		}
		case ART_DAL:
		{
			return "remove fear and cure poison every 5 turns";
		}
		case ART_RINGIL:
		{
            return "frost ball (100) every 40 turns";
		}
		case ART_ANDURIL:
		{
            return "fire ball (72) every 40 turns";
		}
		case ART_FIRESTAR:
		{
            return "large fire ball (72) every 20 turns";
		}
		case ART_FEANOR:
		{
			return "haste self (20+d20 turns) every 200 turns";
		}
		case ART_THEODEN:
		{
            return "drain life (120) every 40 turns";
		}
		case ART_TURMIL:
		{
            return "drain life (90) every 40 turns";
		}
		case ART_CASPANION:
		{
			return "door and trap destruction every 10 turns";
		}
		case ART_AVAVIR:
		{
			return "word of recall every 200 turns";
		}
		case ART_TARATOL:
		{
			return "haste self (20+d20 turns) every 100+d100 turns";
		}
		case ART_ERIRIL:
		{
			return "identify every 10 turns";
		}
		case ART_OLORIN:
		{
			return "probing every 20 turns";
		}
		case ART_EONWE:
		{
			return "mass genocide every 1000 turns";
		}
		case ART_LOTHARANG:
		{
            return "cure wounds (4d8) every 3+d3 turns";
		}
		case ART_CUBRAGOL:
		{
			return "fire branding of bolts every 999 turns";
		}
		case ART_ARUNRUTH:
		{
            return "frost bolt (12d8) every 50 turns";
		}
		case ART_AEGLOS:
		{
            return "frost ball (100) every 35 turns";
		}
		case ART_OROME:
		{
			return "stone to mud every 5 turns";
		}
		case ART_SOULKEEPER:
		{
            return "heal (1000) every 444 turns";
		}
		case ART_BELEGENNON:
		{
			return "phase door every 2 turns";
		}
		case ART_CELEBORN:
		{
			return "genocide every 500 turns";
		}
		case ART_LUTHIEN:
		{
            return "restore life levels every 250 turns";
		}
		case ART_ULMO:
		{
            return "teleport away every 50 turns";
		}
		case ART_COLLUIN:
		{
			return "resistance (20+d20 turns) every 111 turns";
		}
		case ART_HOLCOLLETH:
		{
			return "Sleep II every 55 turns";
		}
		case ART_THINGOL:
		{
			return "recharge item I every 70 turns";
		}
		case ART_COLANNON:
		{
			return "teleport every 45 turns";
		}
		case ART_TOTILA:
		{
			return "confuse monster every 15 turns";
		}
		case ART_CAMMITHRIM:
		{
			return "magic missile (2d6) every 2 turns";
		}
		case ART_PAURHACH:
		{
			return "fire bolt (9d8) every 8+d8 turns";
		}
		case ART_PAURNIMMEN:
		{
			return "frost bolt (6d8) every 7+d7 turns";
		}
		case ART_PAURAEGEN:
		{
			return "lightning bolt (4d8) every 6+d6 turns";
		}
		case ART_PAURNEN:
		{
			return "acid bolt (5d8) every 5+d5 turns";
		}
		case ART_FINGOLFIN:
		{
            return "a magical arrow (150) every 30+d30 turns";
		}
		case ART_HOLHENNETH:
		{
			return "detection every 55+d55 turns";
		}
		case ART_GONDOR:
		{
            return "heal (500) every 250 turns";
		}
		case ART_RAZORBACK:
		{
            return "star ball (150) every 50 turns";
		}
		case ART_BLADETURNER:
		{
			return "berserk rage, bless, and resistance every 400 turns";
		}
        case ART_MEDIATOR:
        {
            return "star ball (150) every 50 turns";
        }
		case ART_GALADRIEL:
		{
			return "illumination every 10+d10 turns";
		}
		case ART_ELENDIL:
		{
			return "magic mapping every 50+d50 turns";
		}
		case ART_THRAIN:
		{
            return "detection every 30+d30 turns";
        }
        case ART_PALANTIR:
        {
            return "clairvoyance every 50+d50 turns";
		}
		case ART_INGWE:
		{
            return "dispel evil (x5) every 50+d50 turns";
		}
		case ART_CARLAMMAS:
		{
			return "protection from evil every 225+d225 turns";
		}
		case ART_TULKAS:
		{
			return "haste self (75+d75 turns) every 150+d150 turns";
		}
		case ART_NARYA:
		{
            return "large fire ball (120) every 20+d20 turns";
		}
		case ART_NENYA:
		{
            return "large frost ball (200) every 20+d20 turns";
		}
		case ART_VILYA:
		{
            return "large lightning ball (250) every 20+d20 turns";
		}
		case ART_POWER:
		{
            return "bizarre things every 30+d30 turns";
        }
        case ART_ELESSAR:
        {
            return "heal (500) every 200 turns";
        }
        case ART_EVENSTAR:
        {
            return "restore life levels every 150 turns";
        }
        case ART_HIMRING:
        {
            return "protection from evil every 100+d100 turns";
        }
        case ART_GILGALAD:
        {
            return "starlight every 100 turns";
        }
        case ART_EOL:
        {
            return "mana bolt (10d10) every 30+d30 turns";
        }
        case ART_WORMTONGUE:
        {
            return "phase door every 20 turns";
        }
        case ART_HURIN:
        {
            return "berserk rage every 80+d80 turns";
        }
        case ART_GOTHMOG:
        {
            return "large fire ball (120) every 15 turns";
        }
        case ART_UMBAR:
        {
            return "a magical arrow (150) every 20+d20 turns";
        }
        case ART_NAIN:
        {
            return "stone to mud every 2 turns";
        }
        case ART_FUNDIN:
        {
            return "dispel evil (x5) every 100+d100 turns";
        }
        case ART_HARADRIM:
        {
            return "berserk rage every 50 turns";
		}
	}

	/* Some ego items can be activated */
	switch (o_ptr->name2)
	{
		case EGO_CLOAK_LORDLY_RES:
		{
			return "resistance every 150+d50 turns";
		}
	}

	/* The amulet of the moon can be activated for sleep */
	if ((o_ptr->tval == TV_AMULET) && (o_ptr->sval == SV_AMULET_THE_MOON))
		return "sleep monsters every 100+d100 turns";

    /* Some rings can be activated for resistance */
    if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES))
    	return "resist fire every 150+d50 turns";
    if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ACID))
    	return "resist acid every 150+d50 turns";
    if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ICE))
    	return "resist cold every 150+d50 turns";
    if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_LIGHTNING))
    	return "resist lightning every 150+d50 turns";

	/* Require dragon scale mail */
	if (o_ptr->tval != TV_DRAG_ARMOR) return (NULL);

	/* Branch on the sub-type */
	switch (o_ptr->sval)
	{
		case SV_DRAGON_BLUE:
		{
			return "breathe lightning (100) every 450+d450 turns";
		}
		case SV_DRAGON_WHITE:
		{
			return "breathe frost (110) every 450+d450 turns";
		}
		case SV_DRAGON_BLACK:
		{
			return "breathe acid (130) every 450+d450 turns";
		}
		case SV_DRAGON_GREEN:
		{
			return "breathe poison gas (150) every 450+d450 turns";
		}
		case SV_DRAGON_RED:
		{
			return "breathe fire (200) every 450+d450 turns";
		}
		case SV_DRAGON_MULTIHUED:
		{
			return "breathe multi-hued (250) every 225+d225 turns";
		}
		case SV_DRAGON_BRONZE:
		{
			return "breathe confusion (120) every 450+d450 turns";
		}
		case SV_DRAGON_GOLD:
		{
			return "breathe sound (130) every 450+d450 turns";
		}
		case SV_DRAGON_CHAOS:
		{
			return "breathe chaos/disenchant (220) every 300+d300 turns";
		}
		case SV_DRAGON_LAW:
		{
			return "breathe sound/shards (230) every 300+d300 turns";
		}
		case SV_DRAGON_BALANCE:
		{
			return "You breathe balance (250) every 300+d300 turns";
		}
		case SV_DRAGON_SHINING:
		{
			return "breathe light/darkness (200) every 300+d300 turns";
		}
		case SV_DRAGON_POWER:
		{
			return "breathe the elements (300) every 300+d300 turns";
		}
	}


	/* Oops */
	return NULL;
}


/*
 * Describe a "fully identified" item
 */
bool identify_fully_aux(int Ind, object_type *o_ptr)
{
	player_type *p_ptr = Players[Ind];
	int i = 0;

    u32b f1, f2, f3, f4;

	cptr		*info = p_ptr->info;

	/* Clear the info area first. */
	memset(p_ptr->info,0,sizeof(p_ptr->info));

	/* Let the player scroll through this info */
	p_ptr->special_file_type = TRUE;

	/* Extract the flags */
    object_flags(o_ptr, &f1, &f2, &f3, &f4);

	/* Mega-Hack -- describe activation */
	if (f3 & TR3_ACTIVATE)
	{
		info[i++] = "It can be activated for...";
		info[i++] = item_activation(o_ptr);
		info[i++] = "...if it is being worn.";
	}


	/* Hack -- describe lite's */
	if (o_ptr->tval == TV_LITE)
	{
		if (artifact_p(o_ptr) || (o_ptr->sval == SV_LITE_FEANOR))
		{
			info[i++] = "It provides light (radius 3) forever.";
		}
		else if (o_ptr->sval == SV_LITE_DWARVEN)
		{
			info[i++] = "It provides light (radius 2) forever.";
		}
		else if (o_ptr->sval == SV_LITE_LANTERN)
		{
			info[i++] = "It provides light (radius 2) when fueled.";
		}
		else
		{
			info[i++] = "It provides light (radius 1) when fueled.";
		}
	}


	/* And then describe it fully */

	if (f1 & TR1_STR)
	{
		info[i++] = "It affects your strength.";
	}
	if (f1 & TR1_INT)
	{
		info[i++] = "It affects your intelligence.";
	}
	if (f1 & TR1_WIS)
	{
		info[i++] = "It affects your wisdom.";
	}
	if (f1 & TR1_DEX)
	{
		info[i++] = "It affects your dexterity.";
	}
	if (f1 & TR1_CON)
	{
		info[i++] = "It affects your constitution.";
	}
	if (f1 & TR1_CHR)
	{
		info[i++] = "It affects your charisma.";
	}

    if (f1 & TR1_MANA)
    {
	info[i++] = "It affects your mana capacity.";
    }

	if (f1 & TR1_STEALTH)
	{
		info[i++] = "It affects your stealth.";
	}
	if (f1 & TR1_SEARCH)
	{
		info[i++] = "It affects your searching.";
	}
	if (f1 & TR1_INFRA)
	{
		info[i++] = "It affects your infravision.";
	}
	if (f1 & TR1_TUNNEL)
	{
		info[i++] = "It affects your ability to tunnel.";
	}
	if (f1 & TR1_SPEED)
	{
		info[i++] = "It affects your speed.";
	}
	if (f1 & TR1_BLOWS)
	{
		info[i++] = "It affects your attack speed.";
	}

	if (f1 & TR1_BRAND_ACID)
	{
		info[i++] = "It does extra damage from acid.";
	}
	if (f1 & TR1_BRAND_ELEC)
	{
		info[i++] = "It does extra damage from electricity.";
	}
	if (f1 & TR1_BRAND_FIRE)
	{
		info[i++] = "It does extra damage from fire.";
	}
	if (f1 & TR1_BRAND_COLD)
	{
		info[i++] = "It does extra damage from frost.";
	}
    if (f1 & TR1_BRAND_POIS)
    {
        info[i++] = "It does extra damage from poison.";
    }

	if (f1 & TR1_IMPACT)
	{
		info[i++] = "It can cause earthquakes.";
	}

	if (f1 & TR1_KILL_DRAGON)
	{
		info[i++] = "It is a great bane of dragons.";
	}
    if (f1 & TR1_KILL_DEMON)
    {
        info[i++] = "It is a great bane of demons.";
    }
    else if (f1 & TR1_SLAY_DEMON)
	{
		info[i++] = "It strikes at demons with holy wrath.";
	}
    if (f1 & TR1_KILL_UNDEAD)
    {
        info[i++] = "It is a great bane of undead.";
    }
    else if (f1 & TR1_SLAY_UNDEAD)
	{
		info[i++] = "It strikes at undead with holy wrath.";
	}
	if (f1 & TR1_SLAY_EVIL)
	{
		info[i++] = "It fights against evil with holy fury.";
	}
	if (f1 & TR1_SLAY_ANIMAL)
	{
		info[i++] = "It is especially deadly against natural creatures.";
	}

	if (f2 & TR2_SUST_STR)
	{
		info[i++] = "It sustains your strength.";
	}
	if (f2 & TR2_SUST_INT)
	{
		info[i++] = "It sustains your intelligence.";
	}
	if (f2 & TR2_SUST_WIS)
	{
		info[i++] = "It sustains your wisdom.";
	}
	if (f2 & TR2_SUST_DEX)
	{
		info[i++] = "It sustains your dexterity.";
	}
	if (f2 & TR2_SUST_CON)
	{
		info[i++] = "It sustains your constitution.";
	}
	if (f2 & TR2_SUST_CHR)
	{
		info[i++] = "It sustains your charisma.";
	}

	if (f2 & TR2_IM_ACID)
	{
		info[i++] = "It provides immunity to acid.";
	}
	if (f2 & TR2_IM_ELEC)
	{
		info[i++] = "It provides immunity to electricity.";
	}
	if (f2 & TR2_IM_FIRE)
	{
		info[i++] = "It provides immunity to fire.";
	}
	if (f2 & TR2_IM_COLD)
	{
		info[i++] = "It provides immunity to cold.";
	}

    if (f2 & TR2_RES_FEAR)
    {
        info[i++] = "It makes you completely fearless.";
    }

	if (f2 & TR2_FREE_ACT)
	{
		info[i++] = "It provides immunity to paralysis.";
	}
	if (f2 & TR2_HOLD_LIFE)
	{
		info[i++] = "It provides resistance to life draining.";
	}

	if (f2 & TR2_RES_ACID)
	{
		info[i++] = "It provides resistance to acid.";
	}
	if (f2 & TR2_RES_ELEC)
	{
		info[i++] = "It provides resistance to electricity.";
	}
	if (f2 & TR2_RES_FIRE)
	{
		info[i++] = "It provides resistance to fire.";
	}
	if (f2 & TR2_RES_COLD)
	{
		info[i++] = "It provides resistance to cold.";
	}
	if (f2 & TR2_RES_POIS)
	{
		info[i++] = "It provides resistance to poison.";
	}

	if (f2 & TR2_RES_LITE)
	{
		info[i++] = "It provides resistance to light.";
	}
	if (f2 & TR2_RES_DARK)
	{
		info[i++] = "It provides resistance to dark.";
	}

	if (f2 & TR2_RES_BLIND)
	{
		info[i++] = "It provides resistance to blindness.";
	}
	if (f2 & TR2_RES_CONF)
	{
		info[i++] = "It provides resistance to confusion.";
	}
	if (f2 & TR2_RES_SOUND)
	{
		info[i++] = "It provides resistance to sound.";
	}
	if (f2 & TR2_RES_SHARDS)
	{
		info[i++] = "It provides resistance to shards.";
	}

	if (f2 & TR2_RES_NETHER)
	{
		info[i++] = "It provides resistance to nether.";
	}
	if (f2 & TR2_RES_NEXUS)
	{
		info[i++] = "It provides resistance to nexus.";
	}
	if (f2 & TR2_RES_CHAOS)
	{
		info[i++] = "It provides resistance to chaos.";
	}
	if (f2 & TR2_RES_DISEN)
	{
		info[i++] = "It provides resistance to disenchantment.";
	}

	if (f3 & TR3_FEATHER)
	{
		info[i++] = "It induces feather falling.";
	}
	if (f3 & TR3_LITE)
	{
		info[i++] = "It provides permanent light.";
	}
	if (f3 & TR3_SEE_INVIS)
	{
		info[i++] = "It allows you to see invisible monsters.";
	}
    if (f4 & TR4_ESP_ALL)
	{
		info[i++] = "It gives telepathic powers.";
	}
	if (f3 & TR3_SLOW_DIGEST)
	{
		info[i++] = "It slows your metabolism.";
	}
	if (f3 & TR3_REGEN)
	{
		info[i++] = "It speeds your regenerative powers.";
	}

	if (f3 & TR3_XTRA_MIGHT)
	{
		info[i++] = "It fires missiles with extra might.";
	}
	if (f3 & TR3_XTRA_SHOTS)
	{
		info[i++] = "It fires missiles excessively fast.";
	}

	if (f3 & TR3_DRAIN_EXP)
	{
		info[i++] = "It drains experience.";
	}
	if (f3 & TR3_TELEPORT)
	{
		info[i++] = "It induces random teleportation.";
	}
	if (f3 & TR3_AGGRAVATE)
	{
		info[i++] = "It aggravates nearby creatures.";
	}

	if (f3 & TR3_BLESSED)
	{
		info[i++] = "It has been blessed by the gods.";
	}

    if (f3 & TR3_KNOWLEDGE)
    {
	info[i++] = "It identifies all items for you.";
    }

	if (cursed_p(o_ptr))
	{
		if (f3 & TR3_PERMA_CURSE)
		{
			info[i++] = "It is permanently cursed.";
		}
		else if (f3 & TR3_HEAVY_CURSE)
		{
			info[i++] = "It is heavily cursed.";
		}
		else
		{
			info[i++] = "It is cursed.";
		}
	}


	if (f3 & TR3_IGNORE_ACID)
	{
		info[i++] = "It cannot be harmed by acid.";
	}
	if (f3 & TR3_IGNORE_ELEC)
	{
		info[i++] = "It cannot be harmed by electricity.";
	}
	if (f3 & TR3_IGNORE_FIRE)
	{
		info[i++] = "It cannot be harmed by fire.";
	}
	if (f3 & TR3_IGNORE_COLD)
	{
		info[i++] = "It cannot be harmed by cold.";
	}

	/* No special effects */

	if (!i) return (FALSE);

	/* Let the client know it's about to get some info */
	Send_special_other(Ind);

	/* Gave knowledge */
	return (TRUE);
}



/*
 * Convert an inventory index into a one character label
 * Note that the label does NOT distinguish inven/equip.
 */
s16b index_to_label(int i)
{
	/* Indexes for "inven" are easy */
	if (i < INVEN_WIELD) return (I2A(i));

	/* Indexes for "equip" are offset */
	return (I2A(i - INVEN_WIELD));
}


/*
 * Convert a label into the index of an item in the "inven"
 * Return "-1" if the label does not indicate a real item
 */
s16b label_to_inven(int Ind, int c)
{
	player_type *p_ptr = Players[Ind];

	int i;

	/* Convert */
	i = (islower(c) ? A2I(c) : -1);

	/* Verify the index */
	if ((i < 0) || (i > INVEN_PACK)) return (-1);

	/* Empty slots can never be chosen */
	if (!p_ptr->inventory[i].k_idx) return (-1);

	/* Return the index */
	return (i);
}


/*
 * Convert a label into the index of a item in the "equip"
 * Return "-1" if the label does not indicate a real item
 */
s16b label_to_equip(int Ind, int c)
{
	player_type *p_ptr = Players[Ind];

	int i;

	/* Convert */
	i = (islower(c) ? A2I(c) : -1) + INVEN_WIELD;

	/* Verify the index */
	if ((i < INVEN_WIELD) || (i >= INVEN_TOTAL)) return (-1);

	/* Empty slots can never be chosen */
	if (!p_ptr->inventory[i].k_idx) return (-1);

	/* Return the index */
	return (i);
}



/*
 * Determine which equipment slot (if any) an item likes
 */
s16b wield_slot(int Ind, object_type *o_ptr)
{
	player_type *p_ptr = Players[Ind];

	/* Slot for equipment */
	switch (o_ptr->tval)
	{
		case TV_DIGGING:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
        case TV_MSTAFF:
		{
			return (INVEN_WIELD);
		}

		case TV_BOW:
		{
			return (INVEN_BOW);
		}

		case TV_RING:
		{
			/* Use the right hand first */
			if (!p_ptr->inventory[INVEN_RIGHT].k_idx) return (INVEN_RIGHT);

			/* Use the left hand for swapping (by default) */
			return (INVEN_LEFT);
		}

		case TV_AMULET:
		{
			return (INVEN_NECK);
		}

		case TV_LITE:
		{
			return (INVEN_LITE);
		}

		case TV_DRAG_ARMOR:
		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		{
			return (INVEN_BODY);
		}

		case TV_CLOAK:
		{
			return (INVEN_OUTER);
		}

		case TV_SHIELD:
		{
			return (INVEN_ARM);
		}

		case TV_CROWN:
		case TV_HELM:
		{
			return (INVEN_HEAD);
		}

		case TV_GLOVES:
		{
			return (INVEN_HANDS);
		}

		case TV_BOOTS:
		{
			return (INVEN_FEET);
		}
	}

	/* No slot available */
	return (-1);
}


/*
 * Return a string mentioning how a given item is carried
 */
cptr mention_use(int Ind, int i)
{
	player_type *p_ptr = Players[Ind];

	cptr p;

	/* Examine the location */
	switch (i)
	{
		case INVEN_WIELD: p = "Wielding"; break;
		case INVEN_BOW:   p = "Shooting"; break;
		case INVEN_LEFT:  p = "On left hand"; break;
		case INVEN_RIGHT: p = "On right hand"; break;
		case INVEN_NECK:  p = "Around neck"; break;
		case INVEN_LITE:  p = "Light source"; break;
		case INVEN_BODY:  p = "On body"; break;
		case INVEN_OUTER: p = "About body"; break;
		case INVEN_ARM:   p = "On arm"; break;
		case INVEN_HEAD:  p = "On head"; break;
		case INVEN_HANDS: p = "On hands"; break;
		case INVEN_FEET:  p = "On feet"; break;
		default:          p = "In pack"; break;
	}

	/* Hack -- Heavy weapon */
	if (i == INVEN_WIELD)
	{
		object_type *o_ptr;
		o_ptr = &p_ptr->inventory[i];
		if (adj_str_hold[p_ptr->stat_ind[A_STR]] < o_ptr->weight / 10)
		{
			p = "Just lifting";
		}
	}

	/* Hack -- Heavy bow */
	if (i == INVEN_BOW)
	{
		object_type *o_ptr;
		o_ptr = &p_ptr->inventory[i];
		if (adj_str_hold[p_ptr->stat_ind[A_STR]] < o_ptr->weight / 10)
		{
			p = "Just holding";
		}
	}

	/* Return the result */
	return (p);
}


/*
 * Return a string describing how a given item is being worn.
 * Currently, only used for items in the equipment, not inventory.
 */
cptr describe_use(int Ind, int i)
{
	player_type *p_ptr = Players[Ind];

	cptr p;

	switch (i)
	{
		case INVEN_WIELD: p = "attacking monsters with"; break;
		case INVEN_BOW:   p = "shooting missiles with"; break;
		case INVEN_LEFT:  p = "wearing on your left hand"; break;
		case INVEN_RIGHT: p = "wearing on your right hand"; break;
		case INVEN_NECK:  p = "wearing around your neck"; break;
		case INVEN_LITE:  p = "using to light the way"; break;
		case INVEN_BODY:  p = "wearing on your body"; break;
		case INVEN_OUTER: p = "wearing on your back"; break;
		case INVEN_ARM:   p = "wearing on your arm"; break;
		case INVEN_HEAD:  p = "wearing on your head"; break;
		case INVEN_HANDS: p = "wearing on your hands"; break;
		case INVEN_FEET:  p = "wearing on your feet"; break;
		default:          p = "carrying in your pack"; break;
	}

	/* Hack -- Heavy weapon */
	if (i == INVEN_WIELD)
	{
		object_type *o_ptr;
		o_ptr = &p_ptr->inventory[i];
		if (adj_str_hold[p_ptr->stat_ind[A_STR]] < o_ptr->weight / 10)
		{
			p = "just lifting";
		}
	}

	/* Hack -- Heavy bow */
	if (i == INVEN_BOW)
	{
		object_type *o_ptr;
		o_ptr = &p_ptr->inventory[i];
		if (adj_str_hold[p_ptr->stat_ind[A_STR]] < o_ptr->weight / 10)
		{
			p = "just holding";
		}
	}

	/* Return the result */
	return p;
}





/*
 * Check an item against the item tester info
 */
bool item_tester_okay(object_type *o_ptr)
{
	/* Hack -- allow listing empty slots */
	if (item_tester_full) return (TRUE);

	/* Require an item */
	if (!o_ptr->k_idx) return (FALSE);

	/* Hack -- ignore "gold" */
	if (o_ptr->tval == TV_GOLD) return (FALSE);

	/* Check the tval */
	if (item_tester_tval)
	{
		if (!(item_tester_tval == o_ptr->tval)) return (FALSE);
	}

	/* Check the hook */
	if (item_tester_hook)
	{
		if (!(*item_tester_hook)(o_ptr)) return (FALSE);
	}

	/* Assume okay */
	return (TRUE);
}




/*
 * Choice window "shadow" of the "show_inven()" function
 *
 * Note that this function simply sends a text string describing the
 * inventory slot, along with the tval, weight, and position in the inventory
 * to the client --KLJ--
 */
void display_inven(int Ind)
{
	player_type *p_ptr = Players[Ind];

	register	int i, n, z = 0;

	object_type *o_ptr;

	byte	attr = TERM_WHITE;

	char	tmp_val[80];

	char	o_name[80];

	int wgt;


	/* Have the final slot be the FINAL slot */
	z = INVEN_WIELD;

	/* Display the pack */
	for (i = 0; i < z; i++)
	{
		/* Examine the item */
		o_ptr = &p_ptr->inventory[i];

		/* Start with an empty "index" */
		tmp_val[0] = tmp_val[1] = tmp_val[2] = ' ';

		/* Prepare an "index" */
		tmp_val[0] = index_to_label(i);

		/* Obtain an item description */
		object_desc(Ind, o_name, o_ptr, TRUE, 3);

		/* Obtain the length of the description */
		n = strlen(o_name);

		/* Get a color */
		attr = tval_to_attr[o_ptr->tval % 128];

		/* Hack -- fake monochrome */
		if (!use_color) attr = TERM_WHITE;

		/* Display the weight if needed */
		wgt = o_ptr->weight * o_ptr->number;

		/* Send the info to the client */
		Send_inven(Ind, tmp_val[0], attr, wgt, o_ptr->number, o_ptr->tval, o_name);
	}
}



/*
 * Choice window "shadow" of the "show_equip()" function
 */
void display_equip(int Ind)
{
	player_type *p_ptr = Players[Ind];

	register	int i, n;
	object_type *o_ptr;
	byte	attr = TERM_WHITE;

	char	tmp_val[80];

	char	o_name[80];

	int wgt;

	/* Display the equipment */
	for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
	{
		/* Examine the item */
		o_ptr = &p_ptr->inventory[i];

		/* Start with an empty "index" */
		tmp_val[0] = tmp_val[1] = tmp_val[2] = ' ';

		/* Prepare an "index" */
		tmp_val[0] = index_to_label(i);

		/* Obtain an item description */
		object_desc(Ind, o_name, o_ptr, TRUE, 3);

		/* Obtain the length of the description */
		n = strlen(o_name);

		/* Get the color */
		attr = tval_to_attr[o_ptr->tval % 128];

		/* Hack -- fake monochrome */
		if (!use_color) attr = TERM_WHITE;

		/* Display the weight (if needed) */
		wgt = o_ptr->weight * o_ptr->number;

		/* Send the info off */
		Send_equip(Ind, tmp_val[0], attr, wgt, o_ptr->tval, o_name);
	}
}
