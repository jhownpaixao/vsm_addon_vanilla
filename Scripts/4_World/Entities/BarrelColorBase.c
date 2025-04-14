modded class Barrel_ColorBase : Container_Base
{
    //! mesclar caracteristicas do storage para virtual
    override bool CanPutInCargo(EntityAI parent)
    {
        if (!super.CanPutInCargo(parent))
            return false;

        return !m_VSM_HasVirtualItems;
    }

    override bool CanPutIntoHands(EntityAI parent)
    {
        if (!super.CanPutIntoHands(parent))
            return false;

        return !m_VSM_HasVirtualItems;
    }

    override void Open()
    {
        super.Open();

        if (GetGame().IsServer())
            VirtualStorageModule.GetModule().OnLoadVirtualStore(this);
    }

    override void Close()
    {
        if (GetGame().IsServer())
        {
            VirtualStorageModule.GetModule().OnSaveVirtualStore(this);
            VSM_StopAutoClose();
        }
            
        super.Close();
    }

    //! virtualização
    override bool VSM_CanVirtualize()
	{
		return !VSM_IsAttachedOnVehicle();
	}
    
    override bool VSM_IsOpen()
    {
        return IsOpen();
    }

    override void VSM_Open()
    {
        super.VSM_Open();

        if (!VSM_IsOpen())
        {
            Open();
        }
    }

    override void VSM_Close()
	{
        super.VSM_Close();

		if (VSM_IsAttachedOnVehicle())
		{
			VSM_StartAutoClose(); //reinicia ciclo
		}
        else if (VSM_IsOpen())
		{
			if (GetGame().IsServer())
				VirtualStorageModule.GetModule().OnSaveVirtualStore(this);

			m_Openable.Close();
			SetSynchDirty();
		}
	}

    override void EEInit()
	{
        super.EEInit();

		if (GetGame().IsServer())
        {
            VirtualStorageModule.GetModule().OnInitContainer(this);
        }
	}

    override void EEDelete(EntityAI parent)
    {
        super.EEDelete(parent);

        if (GetGame().IsServer())
            VirtualStorageModule.GetModule().OnDeleteContainer(this);
    }

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);
        ctx.Write(m_VSM_HasVirtualItems);
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version))  return false;
        ctx.Read(m_VSM_HasVirtualItems);
        
        return true;
    }

    override void AfterStoreLoad()
    {
        super.AfterStoreLoad();
        VSM_SetHasItems(m_VSM_HasVirtualItems);
    }

    override void VSM_OnBeforeVirtualize()
    {
        super.VSM_OnBeforeVirtualize();
        
        if (!VSM_IsOpen()) //! se estiver fechado, os itens são perdidos, caso seja virtualizado dentro de outro container
            VSM_Open();
    }
}