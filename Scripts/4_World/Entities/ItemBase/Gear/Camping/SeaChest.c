modded class SeaChest
{
    protected ref OpenableBehaviour m_Openable;
    protected bool m_IsOpenedLocal;

    override void InitItemVariables()
    {
        super.InitItemVariables();
        
        m_Openable = new OpenableBehaviour(false);
        RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
    }

    //Behaviour overrides----------------------------------------
    override void OnWasAttached(EntityAI parent, int slot_id)
    {
        super.OnWasAttached(parent, slot_id);

        VSM_Open();
    }

    override void OnWasDetached(EntityAI parent, int slot_id)
    {
        super.OnWasDetached(parent, slot_id);

        VSM_Close();
    }

    override bool CanReceiveItemIntoCargo(EntityAI item)
    {
        if (VSM_CanManipule() || VSM_IsAttachedOnVehicle())
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
        if (VSM_CanManipule() || VSM_IsAttachedOnVehicle())
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

    override bool IsTakeable()
    {
        return true;
    }

    override bool CanPutIntoHands(EntityAI parent)
    {
        return true;
    }

    override void SetActions()
    {
        super.SetActions();

        AddAction(ActionVSM_Open);
        AddAction(ActionVSM_Close);
    }

    //VSM adjustments----------------------------------------
    override bool VSM_IsVirtualizable()
    {
        if(super.VSM_IsVirtualizable())
        {
            //! Não pode ser virtualizado por outro storage se tiver itens virtuais ou for attachment
            return !VSM_HasVirtualItems() && !GetInventory().IsAttachment();
        }
        
        return true;
    }
    
    override bool VSM_CanVirtualize()
    {
        //! Não pode virtualizar se estiver em um veículo
        return !VSM_IsAttachedOnVehicle();
    }

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

    //VSM Wrapper events ----------------------------------------
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

    //! compatibilidade---------------------------------------------------
    override void VSM_OnBeforeVirtualize()
    {
        super.VSM_OnBeforeVirtualize();

        if (!VSM_IsOpen()) //! se estiver fechado, os itens são perdidos, caso seja virtualizado dentro de outro container
            VSM_Open();
    }

    override void VSM_OnBeforeRestoreChildren() 
    {
        super.VSM_OnBeforeRestoreChildren();

        //! abrir para restauarar os filhos (senão bloqueia)
        m_Openable.Open();
        SetSynchDirty();
    }

    override void VSM_OnAfterRestore() 
    {
        super.VSM_OnAfterRestore();
        
        if (!VSM_IsAttachedOnVehicle())
        {
            VSM_Close();
        }
    } 
    //! ---------------------------------------------------
};
