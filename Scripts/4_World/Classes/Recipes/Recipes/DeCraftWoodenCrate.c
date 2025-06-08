modded class DeCraftWoodenCrate extends RecipeBase
{
	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		ItemBase crate;
		Class.CastTo(crate, ingredients[0]);

		if ( crate.GetInventory().CountInventory() > 1 || crate.VSM_HasVirtualItems() )
			return false;

		return true;
	}
};
