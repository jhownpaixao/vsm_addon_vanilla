modded class ActionToggleTentOpen
{

	override void OnExecuteServer( ActionData action_data )
	{
		Object targetObject = action_data.m_Target.GetObject();
		Object targetParent = action_data.m_Target.GetParent();

		if ( targetParent && targetParent.IsInherited(TentBase) ) 
		{
			array<string> selections = new array<string>;
			targetObject.GetActionComponentNameList(action_data.m_Target.GetComponentIndex(), selections);
			
			TentBase tent = TentBase.Cast( targetParent );
			for ( int s = 0; s < selections.Count(); s++)
			{
				if ( tent.CanToggleAnimations(selections[s]) )
				{
					if (tent.VSM_IsProcessing())
					{
						VirtualUtils.SendMessageToPlayer(action_data.m_Player, "STR_VSM_NOT_TOGGLE_CONTAINER");
						return;
					}
					tent.ToggleAnimation( selections[s] );
				}
			}
			
			//regenerate pathgraph
			tent.SetAffectPathgraph( true, false );
			
			if ( tent.CanAffectPathgraph() )
			{
				//Start update
				tent.RegenerateNavmesh();
			}
		}
	}
};