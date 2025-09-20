modded class ShelterBase
{
    protected ref OpenableBehaviour m_Openable;

    override void InitItemVariables()
    {
        m_Openable = new OpenableBehaviour(false);
        RegisterNetSyncVariableBool("m_Openable.m_IsOpened");

        super.InitItemVariables();
    }

    //Behaviour overrides----------------------------------------
    override bool CanReceiveItemIntoCargo(EntityAI item)
    {
        if (VSM_CanManipule())
            return super.CanReceiveItemIntoCargo(item);

        return false;
    }

    override bool CanReleaseCargo(EntityAI cargo)
    {
        if (VSM_CanManipule())
            return super.CanReleaseCargo(cargo);

        return false;
    }

    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
        if (VSM_CanManipule()) 
            return super.CanReceiveAttachment(attachment, slotId);

        return false;
    }

    override bool CanReleaseAttachment(EntityAI attachment)
    {
        if (VSM_CanManipule())
            return super.CanReleaseAttachment(attachment);

        return false;
    }

    override bool CanDisplayAttachmentSlot(int slot_id)
    {
        if (VSM_CanManipule())
            return super.CanDisplayAttachmentSlot(slot_id);

        return false;
    }

    override bool CanDisplayCargo()
    {
        if (VSM_CanManipule())
            return super.CanDisplayCargo();

        return false;
    }

    override void SetActions()
    {
        super.SetActions();

        AddAction(ActionVSM_Open);
        AddAction(ActionVSM_Close);
    }

    //VSM adjustments----------------------------------------
    override bool VSM_IsOpen()
    {
        return m_Openable.IsOpened();
    }

    override void VSM_Open()
    {
        if (VSM_CanOpen())
        {
            super.VSM_Open();
            m_Openable.Open();

            if (GetGame().IsServer() && VSM_IsOpen())
                VirtualStorageModule.GetModule().OnLoadVirtualStore(this);  
        }
    }

    override void VSM_Close()
    {
        if (VSM_CanClose())
        {
            if (GetGame().IsServer())
                VirtualStorageModule.GetModule().OnSaveVirtualStore(this);
            
            super.VSM_Close();
            m_Openable.Close();
        }
    }

    override void EEInit()
    {
        super.EEInit();

        if (GetGame().IsServer())
            VirtualStorageModule.GetModule().OnInitContainer(this);
    }

    override void EEDelete(EntityAI parent)
    {
        super.EEDelete(parent);

        if (GetGame().IsServer())
            VirtualStorageModule.GetModule().OnDeleteContainer(this);
    }

    override void OnDamageDestroyed(int oldLevel)
    {
        super.OnDamageDestroyed(oldLevel);

        if (GetGame().IsServer())
            VirtualStorageModule.GetModule().OnDestroyed(this);
    };

    override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
        if(VirtualStorageModule.GetModule().IsRemoving()) return;

        ctx.Write(m_VSM_HasVirtualItems);
        ctx.Write(m_Openable.IsOpened());
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if(!super.OnStoreLoad(ctx, version)) return false;

        //TODO este seria o certo, mas os servers em produção ainda não tem este recurso e vão falhar
        //TODO primeiro lançar uma att com ele, e na proxima desativa-lo
		// if(VSM_Addon_Vanilla.GetAddon().IsNew()) return true; 

        if(!ctx.Read(m_VSM_HasVirtualItems)) return false;

        
        if(VSM_Addon_Vanilla.GetAddon().GetLastVersion() > VSMAddonVanilla_StorageVersion.V_0000)
        {
            bool state;
            if(!ctx.Read(state)) return false;
            if(state) m_Openable.Open();
        }

        SetSynchDirty();

		return true;
	}
};