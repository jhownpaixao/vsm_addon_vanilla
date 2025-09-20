class VSM_Addon_Vanilla extends VSMAddonBase
{
	override protected string GetAddonId()
    {   
        return "VSM_Addon_Vanilla";
    }
    
    override protected int GetInitVersion()
    {
        return VSMAddonVanilla_StorageVersion.V_0000;
    }

    override protected int GetVersion()
    {
        return VSMAddonVanilla_StorageVersion.CURRENT_VERSION;
    }
};