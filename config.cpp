class CfgPatches
{
	class vsm_addon_vanilla
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Gear_Camping", "VSM_Core"};
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

class CfgVehicles
{
	class TentBase;
	class MediumTent: TentBase
	{
		class DamageSystem
		{
			class DamageZones
			{
				class Entrance
				{
					class Health
					{
						hitpoints=99999999;
					};
				};
			};
		};
	};
	class LargeTent: TentBase
	{
		class DamageSystem
		{
			class DamageZones
			{
				class Entrance
				{
					class Health
					{
						hitpoints=99999999;
					};
				};
			};
		};
	};
	class CarTent: TentBase
	{
		class DamageSystem
		{
			class DamageZones
			{
				class Entrance
				{
					class Health
					{
						hitpoints=99999999;
					};
				};
			};
		};
	};
	class PartyTent: TentBase
	{
		class DamageSystem
		{
			class DamageZones
			{
				class Door_Base
				{
					class Health
					{
						hitpoints=99999999;
					};
				};
			};
		};;
	};
}