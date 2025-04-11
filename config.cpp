class CfgPatches
{
	class vsm_addon_vanilla
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "VSM_Core"};
	};
};

class CfgMods
{
	class VSM_Addon_Vanilla
	{
		dir = "vsm_addon_vanilla";
		picture = "";
		action = "";
		hideName = 0;
		name = "VSM Vanilla Addon";
		hidePicture = 0;
		credits = "Jhonnata Paixão";
		author = "iTzChronuZ";
		authorID = "76561199441861559";
		version = "1.0.0";
		extra = 0;
		type = "mod";
		dependencies[]= {"World"};

		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"vsm_addon_vanilla/Scripts/4_World"};
			};
		};
	};
};