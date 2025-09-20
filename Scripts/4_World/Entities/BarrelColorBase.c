modded class Barrel_ColorBase : Container_Base
{

    //Behaviour overrides----------------------------------------
    override bool CanPutInCargo(EntityAI parent)
    {
        if(!VSM_HasVirtualItems())
            return super.CanPutInCargo(parent);

        return false;
    }

    override bool CanPutIntoHands(EntityAI parent)
    {
        if(!VSM_HasVirtualItems())
            return super.CanPutIntoHands(parent);

        return false;
    }

    override void Open()
    {
        if (VSM_CanOpen())
        {
            super.Open();

            if (GetGame().IsServer() && VSM_IsOpen())
                VirtualStorageModule.GetModule().OnLoadVirtualStore(this);
            
            VSM_GetAutoCloseBehavior().Start();
        }
    }

    override void Close()
    {
        if (VSM_CanClose())
        {
            if (GetGame().IsServer())
                VirtualStorageModule.GetModule().OnSaveVirtualStore(this);
            
            super.Close();
            VSM_GetAutoCloseBehavior().Stop();
        }
    }

    override bool IsEmpty()
    {
        if (!VSM_HasVirtualItems())
            return super.IsEmpty();

        return false;
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
            VSM_GetAutoCloseBehavior().Start(); //reinicia ciclo
        }
        else if (VSM_IsOpen())
        {
            if (GetGame().IsServer())
                VirtualStorageModule.GetModule().OnSaveVirtualStore(this);

            m_Openable.Close();
        }
    }

    //VSM Wrapper events ----------------------------------------
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

    override void OnDamageDestroyed(int oldLevel)
	{
		super.OnDamageDestroyed(oldLevel);
		if (GetGame().IsServer())
            VirtualStorageModule.GetModule().OnDestroyed(this);
	}

    override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
        if(VirtualStorageModule.GetModule().IsRemoving()) return;

        ctx.Write(m_VSM_HasVirtualItems);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if(!super.OnStoreLoad(ctx, version)) return false;
        
		//TODO este seria o certo, mas os servers em produção ainda não tem este recurso e vão falhar
        //TODO primeiro lançar uma att com ele, e na proxima desativa-lo
		// if(VSM_Addon_Vanilla.GetAddon().IsNew()) return true; 

        if(!ctx.Read(m_VSM_HasVirtualItems)) return false;
        
        SetSynchDirty();

		return true;
	}

    //! compatibilidade---------------------------------------------------
    override void VSM_OnBeforeVirtualize()
    {
        super.VSM_OnBeforeVirtualize();

        //! se estiver fechado, os itens são perdidos, caso seja virtualizado dentro de outro container
        if (!VSM_IsOpen()) 
            VSM_Open();
    }

    override void VSM_OnBeforeRestoreChildren() 
    {
        super.VSM_OnBeforeRestoreChildren();

        //! abrir para restauarar os filhos (senão bloqueia)
        m_Openable.Open();
    }

    override void VSM_OnAfterRestore() 
    {
        super.VSM_OnAfterRestore();
        
        //! após a restauração (como item), se nao for atached em um veículo, fecha para permitir remover de outros containers
        if (!VSM_IsAttachedOnVehicle())
        {
            VSM_Close();
        }
    } 
    //! ---------------------------------------------------
}