modded class TentBase
{
    void TentBase()
    {
        VSM_StartAutoClose();
    }

    //! mesclar caracteristicas do storage para virtual
    override bool CanBePacked()
    {
        if(super.CanBePacked())
        {
            return !m_VSM_HasVirtualItems;
        }
        return false;
    }
    
    override void ToggleAnimation(string selection)
    {
        if(VSM_IsProcessing())
        {
            VirtualUtils.OnLocalPlayerSendMessage("Items are being generated, please wait...");
            return;
        }
          
        super.ToggleAnimation(selection);
        
        if (GetGame().IsServer())
        {
            if (selection.Contains("entrance") || selection.Contains("door"))
            {
                    if (VSM_IsOpen())
                    {
                        if(VSM_HasVirtualItems())
                            VirtualStorageModule.GetModule().OnLoadVirtualStore(this);     
                    }
                    else 
                    {
                        if(!VSM_HasVirtualItems())
                            VirtualStorageModule.GetModule().OnSaveVirtualStore(this);        
                    }
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

    bool CanDisplayAttachmentCategory( string category_name )
	{
		if (VSM_CanManipule())
            return super.CanDisplayAttachmentCategory(category_name);

        return false;
	}

    //! virtualização
    override bool VSM_IsOpen()
    {
        if (!m_OpeningMask)
            return true;

        foreach (ToggleAnimations toggle, bool state: m_ToggleAnimations)
		{
            string toggle_off = toggle.GetToggleOff();
            toggle_off.ToLower();

            if (toggle_off.Contains("entrance") || toggle_off.Contains("door"))
            {
                bool is_closed = m_OpeningMask & toggle.GetOpeningBit();
                if (!is_closed || IsSelectionRuined(toggle_off))
                    return true;
            }
        }

        return false;
    }

    override void VSM_Open()
    {

        if (VSM_IsOpen())
            return;


        foreach (ToggleAnimations toggle, bool state: m_ToggleAnimations)
		{
            string toggle_on = toggle.GetToggleOn();
            toggle_on.ToLower();

            string toggle_off = toggle.GetToggleOff();
            toggle_off.ToLower();

            if (toggle_off.Contains("entrance") || toggle_off.Contains("door"))
            {

                if (!VSM_IsOpen())
                {
                    ToggleAnimation(toggle_off);
                    return;
                }
            }

            if (toggle_on.Contains("entrance") || toggle_on.Contains("door"))
            {
                if (!VSM_IsOpen())
                    ToggleAnimation(toggle_on);
                
            }

            if (VSM_IsOpen())
                break;
        }

    }

    override void VSM_Close()
    {

        if (!VSM_IsOpen())
            return;

        foreach (ToggleAnimations toggle, bool state: m_ToggleAnimations)
		{
            string toggle_on = toggle.GetToggleOn();
            toggle_on.ToLower();

            string toggle_off = toggle.GetToggleOff();
            toggle_off.ToLower();

            if (toggle_off.Contains("entrance") || toggle_off.Contains("door"))
            {

                if (VSM_IsOpen())
                {
                    if (CanToggleAnimations(toggle_off))
                        ToggleAnimation(toggle_off);
                }
            }

            if (toggle_on.Contains("entrance") || toggle_on.Contains("door"))
            {
                if (VSM_IsOpen())
                {
                    if (CanToggleAnimations(toggle_off))
                        ToggleAnimation(toggle_on);
                }
            }

            if (!VSM_IsOpen())
                break;
        }

        if (VSM_IsOpen())
            VSM_StartAutoClose();
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
	};

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

    //! utils
    bool IsSelectionRuined(string selection)
	{
		string zone;
		DamageSystem.GetDamageZoneFromComponentName(this, selection, zone);
		return GetHealthLevel(zone) == GameConstants.STATE_RUINED;
	}

}