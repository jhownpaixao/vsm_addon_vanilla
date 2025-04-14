modded class ShelterBase
{
    protected ref OpenableBehaviour m_Openable;
    protected bool m_IsOpenedLocal;

    void ShelterBase()
    {
        VSM_StartAutoClose();
    }

    override void InitItemVariables()
    {
        m_Openable = new OpenableBehaviour(false);
        RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
    }

    override void SetActions()
    {
        super.SetActions();

        AddAction(ActionVSM_Open);
        AddAction(ActionVSM_Close);
    }

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

    //! virtualização
    override bool VSM_CanVirtualize()
    {
        CarScript carParent = CarScript.Cast(GetHierarchyParent());
        if (carParent)
        {
            InventoryLocation location = new InventoryLocation();
            GetInventory().GetCurrentInventoryLocation(location);

            int slotID = location.GetSlot();
            if (slotID != -1)
                return false;
        }

        return true;
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

        if (VSM_IsOpen())
        {
            if (GetGame().IsServer())
                VirtualStorageModule.GetModule().OnSaveVirtualStore(this);

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

};