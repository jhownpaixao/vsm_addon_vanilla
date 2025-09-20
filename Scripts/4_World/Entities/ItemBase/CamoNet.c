modded class CamoNet
{
    override bool VSM_IsVirtualizable()
    {
        if(super.VSM_IsVirtualizable())
        {
            return !GetInventory().IsAttachment();
        }
        
        return true;
    }


    //!legacy suport
    override bool VSM_CanVirtualize()
    {
        ItemBase parent = ItemBase.Cast(GetHierarchyParent());
        if (parent)
        {
            InventoryLocation location = new InventoryLocation();
            GetInventory().GetCurrentInventoryLocation(location);
            
            int slotID = location.GetSlot();
            if (slotID != -1)
                return false;
        }

        return true;
    }
}