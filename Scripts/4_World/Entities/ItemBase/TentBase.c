modded class TentBase
{
    bool m_VSM_LastOpenState;

    override void InitItemVariables()
    {
        super.InitItemVariables();

        m_VSM_LastOpenState = VSM_IsOpen();
        RegisterNetSyncVariableBool("m_VSM_LastOpenState");
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

    override bool CanDisplayAttachmentCategory( string category_name )
	{
		if (VSM_CanManipule())
            return super.CanDisplayAttachmentCategory(category_name);

        return false;
	}

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
                    {
                        VirtualStorageModule.GetModule().OnLoadVirtualStore(this);
                        VSM_GetAutoCloseBehavior().Start();
                    }
                    else
                    {
                        VirtualStorageModule.GetModule().OnSaveVirtualStore(this);
                        VSM_GetAutoCloseBehavior().Stop();
                    }    
                }
            }
        }
    }
    
    //VSM adjustments----------------------------------------
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

                if ((component.Contains("entrance") || component.Contains("door")) && !is_closed)
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
            {
                ToggleAnimation(component);
            }   
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

        VSM_Open();
        VSM_GetAutoCloseBehavior().Stop();
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

    //Utils----------------------------------------
    bool IsSelectionRuined(string selection)
    {
        string zone;
        DamageSystem.GetDamageZoneFromComponentName(this, selection, zone);
        return GetHealthLevel(zone) == GameConstants.STATE_RUINED;
    }
}