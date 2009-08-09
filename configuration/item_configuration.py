from item_rule import item_rule
from item_constants import *

item_rules = [
	item_rule("Ear"),
	
	item_rule("A Jade Figurine"),
	
	item_rule(group = "Flawless Gem"),
	item_rule(group = "Perfect Gem"),
	
	item_rule("Small Charm"),
	item_rule("Large Charm"),
	item_rule("Grand Charm"),
	
	item_rule("Ring", quality = rare),
	item_rule("Amulet", quality = rare),
	
	#for low level amazon TPPKers with OrtOrtOrtOrtOrtOrt bows
	item_rule("Long Battle Bow", quality = white, sockets = 0, level = 50),
	item_rule("Long War Bow", quality = white, sockets = 0, level = 50),
	item_rule("Long Battle Bow", quality = white, sockets = 6),
	item_rule("Long War Bow", quality = white, sockets = 6),
	
	#Lawbringer sword for A5 mercenaries, as a blocker against Nihlathak - slain monsters rest in peace
	item_rule("Phase Blade", quality = white, sockets = 3),
	item_rule(group = "Elite Two-Handed Sword", quality = white, sockets = 3),
	
	#For the mercenaries of low level PKers
	item_rule("Halberd", quality = white, ethereal = True, sockets = 0, level = 50),
	item_rule("Halberd", quality = white, sockets = 6),
	
	item_rule("War Scythe", quality = white, ethereal = True, sockets = 0, level = 50),
	item_rule("War Scythe", quality = white, sockets = 6),
	
	item_rule(group = "Boots", quality = rare),
	item_rule(group = "Good Belt", quality = rare),
	item_rule(group = "Gloves", quality = rare),
	item_rule(group = "Circlet", quality = rare),
	item_rule(group = "Circlet", quality = magical),
	
	item_rule(group = "Orb", quality = rare),
	
	item_rule(group = "Elite Javelin", quality = rare, ethereal = True),
	item_rule("Winged Axe", quality = rare, ethereal = True),
	item_rule("Colossus Blade", quality = rare, ethereal = True),
	item_rule("Colossus Sword", quality = rare, ethereal = True),
	item_rule("Scissors Suwayyah", quality = rare, ethereal = True),
	item_rule("Runic Talons", quality = rare, ethereal = True),
	
	item_rule("Matriarchal Bow", quality = rare),
	item_rule("Grand Matron Bow", quality = rare),
	
	item_rule("Matriarchal Javelin", quality = rare, ethereal = True),
	item_rule("Matriarchal Pike", quality = rare, ethereal = True),
	item_rule("Matriarchal Spear", quality = rare, ethereal = True),
	
	item_rule(group = "Elite Spear", quality = rare, ethereal = True),
	item_rule(group = "Elite Polearm", quality = rare, ethereal = True),
	
	item_rule("Colossus Crossbow", quality = rare),
	item_rule("Demon Crossbow", quality = rare),
	item_rule("Hydra Bow", quality = rare),
	
	item_rule(group = "Barbarian Helmet", quality = magical, level = 60),
	item_rule(group = "Barbarian Helmet", quality = rare, level = 60),
	
	item_rule("Ring", quality = unique),
	item_rule("Amulet", quality = unique),
	
	item_rule("Ral Rune"),
	item_rule("Ort Rune"),
	item_rule("Thul Rune"),
	item_rule("Amn Rune"),
	item_rule("Sol Rune"),
	item_rule("Shael Rune"),
	item_rule("Dol Rune"),
	item_rule("Hel Rune"),
	item_rule("Io Rune"),
	item_rule("Lum Rune"),
	item_rule("Ko Rune"),
	item_rule("Fal Rune"),
	item_rule("Lem Rune"),
	item_rule("Pul Rune"),
	item_rule("Um Rune"),
	item_rule("Mal Rune"),
	item_rule("Ist Rune"),
	item_rule("Gul Rune"),
	item_rule("Vex Rune"),
	item_rule("Ohm Rune"),
	item_rule("Lo Rune"),
	item_rule("Sur Rune"),
	item_rule("Ber Rune"),
	item_rule("Jah Rune"),
	item_rule("Cham Rune"),
	item_rule("Zod Rune"),
	
	item_rule("Jewel", quality = magical),
	item_rule("Jewel", quality = rare),
	
	item_rule("Amulet", quality = set),
	
	item_rule("Battle Boots", quality = set),
	item_rule("Shadow Plate", quality = set),
	
	item_rule("Colossus Blade", quality = set),
	item_rule("Mythical Sword", quality = set),
	
	item_rule("War Hat", quality = set),
	item_rule("Studded Leather", quality = set),
	
	item_rule("Corona", quality = set),
	item_rule("Caduceus", quality = set),
	item_rule("Vortex Shield", quality = set),
	
	item_rule("Sacred Armor", quality = set),
	item_rule("War Belt", quality = set),
	item_rule("War Boots", quality = set),
	
	item_rule("Diadem", quality = set),
	item_rule("Grand Matron Bow", quality = set),
	item_rule("Kraken Shell", quality = set),
	item_rule("Battle Gauntlets", quality = set),
	#item_rule("Sharkskin Belt", quality = set),
	
	item_rule("Scissors Suwayyah", quality = set),
	item_rule("Loricated Mail", quality = set),
	item_rule("Mesh Boots", quality = set),
	
	item_rule("Hellforge Plate", quality = set),
	
	item_rule("Swirling Crystal", quality = set),
	item_rule("Death Mask", quality = set),
	item_rule("Lacquered Plate", quality = set),
	item_rule("Mesh Belt", quality = set),
	
	item_rule("Bone Visage", quality = set),
	item_rule("Troll Belt", quality = set),
	
	item_rule("Small Charm", quality = unique),
	item_rule("Grand Charm", quality = unique),
	
	item_rule("Jewel", quality = unique),
	
	item_rule("Ring", quality = unique),
	item_rule("Jewel", quality = unique),
	
	item_rule("Tiara", quality = unique),
	item_rule("Diadem", quality = unique),
	
	item_rule("Ceremonial Javelin", quality = unique),
	item_rule("Matriarchal Javelin", quality = unique),
	
	item_rule("Battle Cestus", quality = unique),
	item_rule("Greater Talons", quality = unique),
	item_rule("Wrist Sword", quality = unique),
	item_rule("Feral Claws", quality = unique),
	
	item_rule("Hierophant Trophy", quality = unique),
	item_rule("Bloodlord Skull", quality = unique),
	item_rule("Succubus Skull", quality = unique),
	
	item_rule("Slayer Guard", quality = unique),
	item_rule("Fury Visor", quality = unique),
	item_rule("Destroyer Helm", quality = unique),
	item_rule("Conqueror Crown", quality = unique),
	
	item_rule("Swirling Crystal", quality = unique),
	item_rule("Eldritch Orb", quality = unique),
	item_rule("Dimensional Shard", quality = unique),
	
	item_rule("Totemic Mask", quality = unique),
	item_rule("Blood Spirit", quality = unique),
	item_rule("Sky Spirit", quality = unique),
	item_rule("Earth Spirit", quality = unique),
	
	item_rule("Sacred Rondache", quality = unique),
	item_rule("Gilded Shield", quality = unique),
	item_rule("Zakarum Shield", quality = unique),
	
	item_rule("Grim Shield", quality = unique),
	
	item_rule("Cap", quality = unique),
	item_rule("Skull Cap", quality = unique),
	item_rule("Full Helm", quality = unique),
	item_rule("Mask", quality = unique),
	
	item_rule("Studded Leather", quality = unique),
	item_rule("Ring Mail", quality = unique),
	
	item_rule("Heavy Gloves", quality = unique),
	item_rule("Chain Gloves", quality = unique),
	item_rule("Light Gauntlets", quality = unique),
	item_rule("Gauntlets", quality = unique),
	
	item_rule("Boots", quality = unique),
	item_rule("Greaves", quality = unique),
	
	item_rule("Sash", quality = unique),
	item_rule("Belt", quality = unique),
	item_rule("Heavy Belt", quality = unique),
	
	item_rule("Long Bow", quality = unique),
	item_rule("Dagger", quality = unique),
	item_rule("Dirk", quality = unique),
	item_rule("Club", quality = unique),
	item_rule("Mace", quality = unique),
	item_rule("Scimitar", quality = unique),
	
	item_rule("War Hat", quality = unique),
	item_rule("Sallet", quality = unique),
	item_rule("Casque", quality = unique),
	item_rule("Winged Helm", quality = unique),
	
	item_rule("Grand Crown", quality = unique),
	item_rule("Grim Helm", quality = unique),
	
	item_rule("Serpentskin Armor", quality = unique),
	item_rule("Cuirass", quality = unique),
	item_rule("Mesh Armor", quality = unique),
	item_rule("Russet Armor", quality = unique),
	item_rule("Templar Coat", quality = unique),
	item_rule("Chaos Armor", quality = unique),
	
	item_rule("Round Shield", quality = unique),
	
	item_rule("Battle Gauntlets", quality = unique),
	
	item_rule("Demonhide Boots", quality = unique),
	item_rule("Sharkskin Boots", quality = unique),
	item_rule("Mesh Boots", quality = unique),
	item_rule("Battle Boots", quality = unique),
	item_rule("War Boots", quality = unique),
	
	item_rule("Demonhide Sash", quality = unique),
	item_rule("Sharkskin Belt", quality = unique),
	item_rule("Mesh Belt", quality = unique),
	item_rule("Battle Belt", quality = unique),
	item_rule("War Belt", quality = unique),
	item_rule("Ballista", quality = unique),
	
	item_rule("Burnt Wand", quality = unique),
	item_rule("Tomb Wand", quality = unique),
	
	item_rule("Shako", quality = unique),
	item_rule("Spired Helm", quality = unique),
	item_rule("Demonhead", quality = unique),
	item_rule("Corona", quality = unique),
	item_rule("Bone Visage", quality = unique),
	
	item_rule("Dusk Shroud", quality = unique),
	item_rule("Wire Fleece", quality = unique),
	item_rule("Balrog Skin", quality = unique),
	item_rule("Kraken Shell", quality = unique),
	item_rule("Shadow Plate", quality = unique),
	item_rule("Sacred Armor", quality = unique),
	
	item_rule("Monarch", quality = unique),
	item_rule("Troll Nest", quality = unique),
	item_rule("Vampirebone Gloves", quality = unique),
	item_rule("Vambraces", quality = unique),
	item_rule("Ogre Gauntlets", quality = unique),
	
	item_rule("Scarabshell Boots", quality = unique),
	item_rule("Boneweave Boots", quality = unique),
	item_rule("Myrmidon Greaves", quality = unique),
	
	item_rule("Spiderweb Sash", quality = unique),
	item_rule("Vampirefang Belt", quality = unique),
	item_rule("Mithril Coil", quality = unique),
	item_rule("Ettin Axe", quality = unique, ethereal = True),
	item_rule("Champion Axe", quality = unique),
	item_rule("Glorious Axe", quality = unique),
	
	item_rule("Crusader Bow", quality = unique),
	item_rule("Hydra Bow", quality = unique),
	item_rule("Colossus Crossbow", quality = unique),
	item_rule("Bone Knife", quality = unique),
	
	item_rule("Flying Axe", quality = unique),
	item_rule("Balrog Spear", quality = unique),
	item_rule("Ghost Glaive", quality = unique),
	item_rule("Winged Harpoon", quality = unique),
	item_rule("Legendary Mallet", quality = unique),
	item_rule("Thunder Maul", quality = unique),
	
	item_rule("Tyrant Club", quality = unique),
	item_rule("Zweihander", quality = unique),
	item_rule("Basinet", quality = unique),
	
	item_rule("Ogre Axe", quality = unique),
	item_rule("Thresher", quality = unique),
	item_rule("Cryptic Axe", quality = unique),
	item_rule("Giant Thresher", quality = unique),
	
	item_rule("Hyperion Spear", quality = unique, ethereal = True),
	item_rule("War Pike", quality = unique),
	
	item_rule("Elder Staff", quality = unique),
	item_rule("Archon Staff", quality = unique),
	
	item_rule("Phase Blade", quality = unique),
	item_rule("Champion Sword", quality = unique),
	item_rule("Colossus Blade", quality = unique),
	
	item_rule("Winged Axe", quality = unique, ethereal = True),
	
	item_rule("Lich Wand", quality = unique),
	item_rule("Unearthed Wand", quality = unique),
	
	item_rule("Large Shield", quality = superior, sockets = 3),
	
	item_rule("Flail", quality = superior, sockets = [0, 4, 5]),
	
	item_rule("Crystal Sword", quality = white, sockets = 4),
	#item_rule("Broad Sword", quality = white, sockets = [0, 4]),
	
	item_rule("Monarch", quality = white, sockets = [0, 4]),
	
	item_rule(group = "Barbarian Helmet", quality = white, sockets = [0, 3], level = 40),
	
	item_rule(group = "Barbarian Helmet", quality = superior, sockets = 3),
	
	item_rule(group = "Pelt", quality = rare),
	item_rule(group = "Pelt", quality = white, sockets = [0, 3], level = 40),
	
	item_rule("Mask", quality = white, sockets = 0, level = 30),
	item_rule("Mask", sockets = 3),
	
	item_rule("Breast Plate", quality = superior, sockets = 3),
	
	item_rule(group = "Elite Armor", ethereal = True, sockets = [0, 3, 4]),
	
	item_rule(group = "Elite Armor", quality = superior, sockets = [0, 3, 4]),
	
	item_rule("Gothic Plate", quality = white, sockets = [0, 4]),
	
	item_rule("Crown", quality = white, sockets = 3),
	item_rule("Great Helm", quality = white, sockets = 3),
	
	item_rule(group = "Paladin Shield", quality = white, sockets = 4),
	item_rule(group = "Paladin Shield", quality = superior, sockets = 0),
	
	item_rule("Bill", quality = white, sockets = [0, 4]),
	
	item_rule("Battle Scythe", quality = white, sockets = 4),
	item_rule("Partizan", quality = white, sockets = 4),
	item_rule("Bec-De-Corbin", quality = white, sockets = 4),
	item_rule("Grim Scythe", quality = white, sockets = 4),
	
	#item_rule("Voulge", quality = white, sockets = [0, 4]),
	item_rule("Scythe", quality = white, sockets = 4),
	item_rule("Poleaxe", quality = white, sockets = 4),
	item_rule("Halberd", quality = white, sockets = 4),
	item_rule("War Scythe", quality = white, sockets = 4),
	
	item_rule(group = "Elite Polearm", ethereal = True, sockets = 0),
	item_rule(group = "Elite Polearm", quality = white, sockets = [4, 5]),
	
	item_rule(group = "Exceptional Polearm", quality = white, sockets = 4),
	
	item_rule("Berserker Axe", quality = superior, ethereal = True, sockets = [0, 6]),
	
	item_rule("Colossus Blade", quality = superior, ethereal = True, sockets = [0, 6]),
	
	item_rule("Matriarchal Bow", quality = superior, sockets = [0, 4]),
	
	item_rule("Grand Matron Bow", quality = superior, sockets = [0, 4]),
	
	item_rule("Chaos Armor", quality = set),
	item_rule("Ornate Plate", quality = set),
	
	item_rule("Great Helm", quality = set),
	item_rule("Gothic Plate", quality = set),
	item_rule("Greaves", quality = set),
	item_rule("Plated Belt", quality = set),
	item_rule("Gauntlets", quality = set),
	
	item_rule("Mithril Coil", quality = set),
	
	item_rule(group = "Wand", quality = white, ethereal = False, sockets = [0, 2], level = 30)
]