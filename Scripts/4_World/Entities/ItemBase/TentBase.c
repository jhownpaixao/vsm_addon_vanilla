modded class TentBase
{
    bool m_VSM_LastOpenState;

    void TentBase()
    {
        VSM_StartAutoClose();
    }

    override void InitItemVariables()
    {
        super.InitItemVariables();

        m_VSM_LastOpenState = VSM_IsOpen();
        RegisterNetSyncVariableBool("m_VSM_LastOpenState");
    }

    //! mesclar caracteristicas do storage para virtual
    override bool CanBePacked()
    {
        if (super.CanBePacked())
        {
            return !m_VSM_HasVirtualItems;
        }
        return false;
    }

    override void ToggleAnimation(string selection)
    {
        if (VSM_IsProcessing())
            return;

        super.ToggleAnimation(selection);

        if (GetGame().IsServer())
        {
            if (m_VSM_LastOpenState != VSM_IsOpen())
            {
                m_VSM_LastOpenState = VSM_IsOpen();
                if (selection.Contains("entrance") || selection.Contains("door"))
                {
                    if (VSM_IsOpen())
                        VirtualStorageModule.GetModule().OnLoadVirtualStore(this);
                    else
                        VirtualStorageModule.GetModule().OnSaveVirtualStore(this);
                }
                SetSynchDirty();
            }
        }

    }

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
        //!desativar por enquanto, está impedindo a criação de attachments mesmo vindo do módulo de virtualização
        //TODO: formular um método de criação dos attachments apartir do módulo, ao mesmo tempo que não permite o player mexer...
        if (VSM_IsOpen() /* && !VSM_CanManipule() */)
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
        if (VSM_IsOpen())
            return super.CanDisplayCargo();

        return false;
    }

    override bool CanDisplayAttachmentCategory(string category_name)
    {
        if (VSM_CanManipule())
            return super.CanDisplayAttachmentCategory(category_name);

        return false;
    }

    //! virtualização
    override bool VSM_IsOpen()
    {
        if (m_State != PACKED)
        {
            bool is_closed;
            string component;
            ToggleAnimations toggle;

            for (int i = 0; i < m_ToggleAnimations.Count(); i++)
            {
                toggle = m_ToggleAnimations.GetKey(i);
                is_closed = m_OpeningMask & toggle.GetOpeningBit();
                component = toggle.GetToggleOff(); //either one works
                component.ToLower();

                if (!is_closed)
                    return true;
            }
        }

        return false;
    }

    override void VSM_Open()
    {

        if (VSM_IsOpen())
            return;

        bool is_closed;
        string component;
        ToggleAnimations toggle;

        for (int i = 0; i < m_ToggleAnimations.Count(); i++)
        {
            toggle = m_ToggleAnimations.GetKey(i);
            is_closed = m_OpeningMask & toggle.GetOpeningBit();
            component = toggle.GetToggleOff(); //either one works
            component.ToLower();

            if (is_closed)
                ToggleAnimation(component);
        }
    }

    override void VSM_Close()
    {
        if (!VSM_IsOpen())
            return;

        bool is_closed;
        string component;
        ToggleAnimations toggle;

        for (int i = 0; i < m_ToggleAnimations.Count(); i++)
        {
            toggle = m_ToggleAnimations.GetKey(i);
            is_closed = m_OpeningMask & toggle.GetOpeningBit();
            component = toggle.GetToggleOff(); //either one works
            component.ToLower();

            if (!is_closed)
            {
                ToggleAnimation(component);
            }   
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

    override void OnDamageDestroyed(int oldLevel)
    {
        super.OnDamageDestroyed(oldLevel);
        if (GetGame().IsServer())
            VirtualStorageModule.GetModule().OnDestroyed(this);
    }

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);

        if (!VirtualStorageModule.GetModule().IsRemoving())
            ctx.Write(m_VSM_HasVirtualItems);
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version))
            return false;

        if (!VirtualStorageModule.GetModule().IsNew())
            ctx.Read(m_VSM_HasVirtualItems);

        return true;
    }

    override void AfterStoreLoad()
    {
        super.AfterStoreLoad();
        VSM_SetHasItems(m_VSM_HasVirtualItems);
    }

    //! utils
    bool IsSelectionRuined(string selection)
    {
        string zone;
        DamageSystem.GetDamageZoneFromComponentName(this, selection, zone);
        return GetHealthLevel(zone) == GameConstants.STATE_RUINED;
    }
}