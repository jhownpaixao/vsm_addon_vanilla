modded class TentBase
{
    protected bool m_LastOpen;

    void TentBase()
    {
        VSM_StartAutoClose();
        m_LastOpen = VSM_IsOpen();
    }

    //! mesclar caracteristicas do storage para virtual
    override void ToggleAnimation(string selection)
    {
        super.ToggleAnimation(selection);
        // Print("ToggleAnimation selection=" + selection + " VSM_IsOpen()=" + VSM_IsOpen());
        if (GetGame().IsServer())
        {
            if (selection.Contains("entrance") || selection.Contains("door"))
            {
                if (VSM_IsOpen() != m_LastOpen)
                {
                    if (VSM_IsOpen())
                    {
                        VirtualStorageModule.GetModule().OnLoadVirtualStore(this);
                    }
                    else
                    {
                        VirtualStorageModule.GetModule().OnSaveVirtualStore(this);
                    }

                    m_LastOpen = VSM_IsOpen();
                }

            }
        }
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

        // Print("Abrindo tenda");
        foreach (ToggleAnimations toggle, bool state: m_ToggleAnimations)
		{
            string toggle_on = toggle.GetToggleOn();
            toggle_on.ToLower();

            string toggle_off = toggle.GetToggleOff();
            toggle_off.ToLower();

            if (toggle_off.Contains("entrance") || toggle_off.Contains("door"))
            {
                // Print("toggle_off =" + toggle_off);

                if (!VSM_IsOpen())
                {
                    // Print("abrindo com toggle_off =" + toggle_off);
                    ToggleAnimation(toggle_off);
                    return;
                }
            }

            if (toggle_on.Contains("entrance") || toggle_on.Contains("door"))
            {
                // Print("toggle_on =" + toggle_on);

                if (!VSM_IsOpen())
                {
                    // Print("abrindo com toggle_on =" + toggle_on);

                    ToggleAnimation(toggle_on);
                }
            }

            if (VSM_IsOpen())
                break;
        }

    }

    override void VSM_Close()
    {
        if (!VSM_IsOpen())
            return;

        // Print("Fechando tenda " + GetType());

        foreach (ToggleAnimations toggle, bool state: m_ToggleAnimations)
		{
            string toggle_on = toggle.GetToggleOn();
            toggle_on.ToLower();

            string toggle_off = toggle.GetToggleOff();
            toggle_off.ToLower();

            if (toggle_off.Contains("entrance") || toggle_off.Contains("door"))
            {
                // Print("toggle_off =" + toggle_off);

                if (VSM_IsOpen())
                {
                    // Print("toggle_off fechando =" + toggle_off);
                    if (CanToggleAnimations(toggle_off))
                        ToggleAnimation(toggle_off);
                }
            }

            if (toggle_on.Contains("entrance") || toggle_on.Contains("door"))
            {
                // Print("toggle_on =" + toggle_off);

                if (VSM_IsOpen())
                {
                    // Print("toggle_on fechando =" + toggle_off);
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