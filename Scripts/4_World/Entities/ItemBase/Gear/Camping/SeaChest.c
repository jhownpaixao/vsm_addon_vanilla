modded class SeaChest
{
    protected ref OpenableBehaviour m_Openable;
    protected bool m_IsOpenedLocal;

    void SeaChest()
    {
        VSM_StartAutoClose();
    }

    override void InitItemVariables()
    {
        m_Openable = new OpenableBehaviour(false);
        RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
    }

    //adciona ações de abrir e fechar
    override void SetActions()
    {
        super.SetActions();

        AddAction(ActionVSM_Open);
        AddAction(ActionVSM_Close);
    }

    // ao colocar em algum veiculo (por exemplo), restaura os itens
    override void OnWasAttached(EntityAI parent, int slot_id)
    {
        super.OnWasAttached(parent, slot_id);

        VSM_Open();
    }

    // ao remover de algum veiculo (por exemplo), fecha
    override void OnWasDetached(EntityAI parent, int slot_id)
    {
        super.OnWasDetached(parent, slot_id);

        VSM_Close();
    }

    // so aceita itens se estiver aberto
    override bool CanReceiveItemIntoCargo(EntityAI item)
    {
        if (VSM_IsOpen())
            return super.CanReceiveItemIntoCargo(item);

        return false;
    }
    
    override bool CanReleaseCargo(EntityAI cargo)
    {
        return VSM_IsOpen();
    }

    override bool CanDisplayAttachmentSlot(int slot_id)
    {
        return VSM_IsOpen();
    }

    override bool CanDisplayCargo()
    {
        return VSM_IsOpen();
    }

    override bool CanReleaseAttachment(EntityAI attachment)
    {
        return VSM_IsOpen();
    }

    override bool IsTakeable()
    {
        return true;
    }

    override bool CanPutIntoHands(EntityAI parent)
    {
        return true;
    }

    //! virtualização
    override bool VSM_CanVirtualize()
	{
		return !VSM_IsAttachedOnVehicle();
	}

    override bool VSM_IsOpen()
    {
        return m_Openable.IsOpened();
    }

    override void VSM_Open()
    {
        super.VSM_Open();

        if (!VSM_IsOpen())
        {
            m_Openable.Open();

            if (GetGame().IsServer())
                VirtualStorageModule.GetModule().OnLoadVirtualStore(this);
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
            VirtualStorageModule.GetModule().OnInitContainer(this); 
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
        if (!super.OnStoreLoad(ctx, version))
            return false;
        ctx.Read(m_VSM_HasVirtualItems)
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
};
