modded class ActionCloseBarrel
{
	override void OnExecuteServer( ActionData action_data )
	{
		Object target_object = action_data.m_Target.GetObject();
		Barrel_ColorBase ntarget = Barrel_ColorBase.Cast( target_object );
		
		if( ntarget )
		{
			if (!ntarget.VSM_CanClose())
			{
				VirtualUtils.SendMessageToPlayer(action_data.m_Player, "STR_VSM_NOT_CLOSE_CONTAINER");
				return;
			}
			
			ntarget.DetermineAction(action_data.m_Player);
			if (ntarget.GetBarrelCloseSoundset() != string.Empty)
				ntarget.StartItemSoundServer(SoundConstants.ITEM_BARREL_CLOSE);
		}
	}
}