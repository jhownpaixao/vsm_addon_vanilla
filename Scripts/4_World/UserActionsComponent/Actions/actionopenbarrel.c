modded class ActionOpenBarrel
{
	override void OnExecuteServer( ActionData action_data )
	{
		Object target_object = action_data.m_Target.GetObject();
		Barrel_ColorBase ntarget = Barrel_ColorBase.Cast( target_object );
		if( ntarget )
		{
			if (!ntarget.VSM_CanOpen())
			{
				VirtualUtils.SendMessageToPlayer(action_data.m_Player, "STR_VSM_NOT_OPEN_CONTAINER");
				return;
			}

			ntarget.Open();
			
			if (ntarget.GetBarrelOpenSoundset() != string.Empty)
				ntarget.StartItemSoundServer(SoundConstants.ITEM_BARREL_OPEN);
		}
	}
}