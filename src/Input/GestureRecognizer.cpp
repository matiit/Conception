#include "../Main.h"

bool IsPointerPointingDeactivationEvent(const InputEvent & InputEvent)
{
	return (   nullptr != InputEvent.m_Pointer
			&& Pointer::VirtualCategory::POINTING == InputEvent.m_Pointer->GetVirtualCategory()
			&& InputEvent.HasType(InputEvent::EventType::POINTER_DEACTIVATION));
}

const sint32 TapRadius = 3;
const double TapTime = 0.200;
const double DoubleTapTime = 2 * TapTime;

uint8 MatchDoubleTap(const InputEventQueue::Queue & Queue, InputEventQueue::Queue::const_iterator & InputEventIterator)
{
	if (Queue.end() == InputEventIterator)
		return 1;

	if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, 0, true>(*InputEventIterator))
	{
		Vector2n DownPosition(InputEventIterator->m_PostEventState.GetAxisState(0).GetPosition(), InputEventIterator->m_PostEventState.GetAxisState(1).GetPosition());
		auto DownTime = InputEventIterator->GetTimestamp();

		uint8 NumberOfTaps = 0;

		auto InputEventIterator2 = InputEventIterator;
		++InputEventIterator2;
		for (; Queue.end() != InputEventIterator2; ++InputEventIterator2)
		{
			if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, 0, false>(*InputEventIterator2))
			{
				Vector2n UpPosition(InputEventIterator2->m_PostEventState.GetAxisState(0).GetPosition(), InputEventIterator2->m_PostEventState.GetAxisState(1).GetPosition());
				auto UpTime = InputEventIterator2->GetTimestamp();

				if (   (UpPosition - DownPosition).LengthSquared() <= (TapRadius * TapRadius)
					&& (UpTime - DownTime) <= DoubleTapTime)
				{
					if (1 == NumberOfTaps)
					{
						++InputEventIterator2;
						InputEventIterator = InputEventIterator2;
						return 2;
					}
					else
						++NumberOfTaps;
				}
				else
				{
					return 0;
				}
			}
			else if (IsPointerPointingMoveEvent<0>(*InputEventIterator2))
			{
				Vector2n MovePosition(InputEventIterator2->m_PostEventState.GetAxisState(0).GetPosition(), InputEventIterator2->m_PostEventState.GetAxisState(1).GetPosition());
				auto NewTime = InputEventIterator2->GetTimestamp();

				if (   (MovePosition - DownPosition).LengthSquared() <= (TapRadius * TapRadius)
					&& (NewTime - DownTime) <= TapTime)
				{}
				else
				{
					return 0;
				}
			}
			else if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, 0, true>(*InputEventIterator2))
			{
				Vector2n DownAgainPosition(InputEventIterator2->m_PostEventState.GetAxisState(0).GetPosition(), InputEventIterator2->m_PostEventState.GetAxisState(1).GetPosition());
				auto NewTime = InputEventIterator2->GetTimestamp();

				if (   (DownAgainPosition - DownPosition).LengthSquared() <= (TapRadius * TapRadius)
					&& (NewTime - DownTime) <= TapTime)
				{}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}

		// Only if there's still a chance a new event can come in time to make a match
		if ((glfwGetTime() - DownTime) <= DoubleTapTime)
		{
			return 1;
		}
	}

	return 0;
}

uint8 MatchTap(const InputEventQueue::Queue & Queue, InputEventQueue::Queue::const_iterator & InputEventIterator)
{
	if (Queue.end() == InputEventIterator)
		return 1;

	if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, 0, true>(*InputEventIterator))
	{
		Vector2n DownPosition(InputEventIterator->m_PostEventState.GetAxisState(0).GetPosition(), InputEventIterator->m_PostEventState.GetAxisState(1).GetPosition());
		auto DownTime = InputEventIterator->GetTimestamp();

		auto InputEventIterator2 = InputEventIterator;
		++InputEventIterator2;
		for (; Queue.end() != InputEventIterator2; ++InputEventIterator2)
		{
			if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, 0, false>(*InputEventIterator2))
			{
				Vector2n UpPosition(InputEventIterator2->m_PostEventState.GetAxisState(0).GetPosition(), InputEventIterator2->m_PostEventState.GetAxisState(1).GetPosition());
				auto UpTime = InputEventIterator2->GetTimestamp();

				if (   (UpPosition - DownPosition).LengthSquared() <= (TapRadius * TapRadius)
					&& (UpTime - DownTime) <= TapTime)
				{
					++InputEventIterator2;
					InputEventIterator = InputEventIterator2;
					return 2;
				}
				else
				{
					return 0;
				}
			}
			else if (IsPointerPointingMoveEvent<0>(*InputEventIterator2))
			{
				Vector2n MovePosition(InputEventIterator2->m_PostEventState.GetAxisState(0).GetPosition(), InputEventIterator2->m_PostEventState.GetAxisState(1).GetPosition());
				auto NewTime = InputEventIterator2->GetTimestamp();

				if (   (MovePosition - DownPosition).LengthSquared() <= (TapRadius * TapRadius)
					&& (NewTime - DownTime) <= TapTime)
				{}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}

		// Only if there's still a chance a new event can come in time to make a match
		if ((glfwGetTime() - DownTime) <= TapTime)
		{
			return 1;
		}
	}

	return 0;
}

/*MatchResult MatchDown(const InputEventQueue::Queue & Queue, MatchResult LastResult)
{
	if (2 != LastResult.Status)
		return LastResult;

	if (Queue.end() == LastResult.End)
		return MatchResult(1);

	if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, 0, true>(*LastResult.End))
	{
		LastResult.Events.push_back(LastResult.End);
		++LastResult.End;
		return LastResult;
	}

	return MatchResult();
}*/

MatchResult MatchDown(const InputEventQueue::FilteredQueue & Queue, InputEventQueue::FilteredQueue::const_iterator InputEventIterator)
{
	if (Queue.end() == InputEventIterator)
		return MatchResult(1);

	if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, 0, true>(**InputEventIterator))
	{
		InputEventQueue::FilteredQueue Events;
		Events.push_back(*InputEventIterator);
		++InputEventIterator;
		return MatchResult(InputEventIterator, Events);
	}

	return MatchResult();
}

MatchResult MatchUp(const InputEventQueue::FilteredQueue & Queue, InputEventQueue::FilteredQueue::const_iterator InputEventIterator)
{
	if (Queue.end() == InputEventIterator)
		return MatchResult(1);

	if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, 0, false>(**InputEventIterator))
	{
		InputEventQueue::FilteredQueue Events;
		Events.push_back(*InputEventIterator);
		++InputEventIterator;
		return MatchResult(InputEventIterator, Events);
	}

	return MatchResult();
}

MatchResult MatchManipulationBegin(const InputEventQueue::FilteredQueue & Queue, InputEventQueue::FilteredQueue::const_iterator InputEventIterator, bool InManipulationTEST, bool HitTEST, Input::InputId ButtonId)
{
	if (   false != InManipulationTEST
		|| true != HitTEST)
	{
		return MatchResult();
	}

	if (Queue.end() == InputEventIterator)
		return MatchResult(1);

	if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, true>(**InputEventIterator, ButtonId))
	{
		InputEventQueue::FilteredQueue Events;
		Events.push_back(*InputEventIterator);
		++InputEventIterator;
		return MatchResult(InputEventIterator, Events);
	}

	return MatchResult();
}
MatchResult MatchManipulationUpdate(const InputEventQueue::FilteredQueue & Queue, InputEventQueue::FilteredQueue::const_iterator InputEventIterator, bool InManipulationTEST)
{
	if (true != InManipulationTEST)
		return MatchResult();

	if (Queue.end() == InputEventIterator)
		return MatchResult(1);

	if (IsPointerPointingMoveEvent<0>(**InputEventIterator))
	{
		InputEventQueue::FilteredQueue Events;
		Events.push_back(*InputEventIterator);
		++InputEventIterator;
		return MatchResult(InputEventIterator, Events);
	}

	return MatchResult();
}
MatchResult MatchManipulationEnd(const InputEventQueue::FilteredQueue & Queue, InputEventQueue::FilteredQueue::const_iterator InputEventIterator, bool InManipulationTEST, Input::InputId ButtonId)
{
	if (true != InManipulationTEST)
		return MatchResult();

	if (Queue.end() == InputEventIterator)
		return MatchResult(1);

	if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, false>(**InputEventIterator, ButtonId))
	{
		InputEventQueue::FilteredQueue Events;
		Events.push_back(*InputEventIterator);
		++InputEventIterator;
		return MatchResult(InputEventIterator, Events);
	}

	return MatchResult();
}

MatchResult MatchSpace(const InputEventQueue::FilteredQueue & Queue, InputEventQueue::FilteredQueue::const_iterator InputEventIterator)
{
	if (Queue.end() == InputEventIterator)
		return MatchResult(1);

	if (IsPointerButtonEvent<Pointer::VirtualCategory::TYPING, GLFW_KEY_SPACE, true>(**InputEventIterator))
	{
		InputEventQueue::FilteredQueue Events;
		Events.push_back(*InputEventIterator);
		++InputEventIterator;
		return MatchResult(InputEventIterator, Events);
	}

	return MatchResult();
}

MatchResult MatchTap2(const InputEventQueue::FilteredQueue & Queue, InputEventQueue::FilteredQueue::const_iterator InputEventIterator)
{
	auto Match = MatchDown(Queue, InputEventIterator);
	if (2 == Match.Status)
	{
		Vector2n DownPosition((*InputEventIterator)->m_PostEventState.GetAxisState(0).GetPosition(), (*InputEventIterator)->m_PostEventState.GetAxisState(1).GetPosition());
		auto DownTime = (*InputEventIterator)->GetTimestamp();
		auto PointingPointer = (*InputEventIterator)->m_Pointer;

		auto FilteredQueue2 = InputEventQueue::FilterByPointer(InputEventQueue::CreateFilteredQueue(Queue, Match.End), PointingPointer);
		for (auto InputEventIterator2 = FilteredQueue2.begin(); FilteredQueue2.end() != InputEventIterator2; ++InputEventIterator2)
		//for (auto & InputEventIterator2 : InputEventQueue::FilterByPointer(InputEventQueue::CreateFilteredQueue(Queue, DownMatch.End), PointingPointer))
		//for (auto InputEventIterator2 = DownMatch.End; Queue.end() != InputEventIterator2; ++InputEventIterator2)
		{
			Match.Events.push_back(*InputEventIterator2);

			if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, 0, false>(**InputEventIterator2))
			{
				Vector2n UpPosition((*InputEventIterator2)->m_PostEventState.GetAxisState(0).GetPosition(), (*InputEventIterator2)->m_PostEventState.GetAxisState(1).GetPosition());
				auto UpTime = (*InputEventIterator2)->GetTimestamp();

				if (   (UpPosition - DownPosition).LengthSquared() <= (TapRadius * TapRadius)
					&& (UpTime - DownTime) <= TapTime)
				{
					++InputEventIterator2;
					return MatchResult(InputEventIterator2, Match.Events);
				}
				else
				{
					return MatchResult();
				}
			}
			else if (IsPointerPointingMoveEvent<0>(**InputEventIterator2))
			{
				Vector2n MovePosition((*InputEventIterator2)->m_PostEventState.GetAxisState(0).GetPosition(), (*InputEventIterator2)->m_PostEventState.GetAxisState(1).GetPosition());
				auto NewTime = (*InputEventIterator2)->GetTimestamp();

				if (   (MovePosition - DownPosition).LengthSquared() <= (TapRadius * TapRadius)
					&& (NewTime - DownTime) <= TapTime)
				{}
				else
				{
					return MatchResult();
				}
			}
			else
			{
				return MatchResult();
			}
		}

		// Only if there's still a chance a new event can come in time to make a match
		if ((glfwGetTime() - DownTime) <= TapTime)
		{
			Match.Status = 1;
			return Match;
		}
	}
	else
	{
		return Match;
	}

	return MatchResult();
}

MatchResult MatchDoubleTap2(const InputEventQueue::FilteredQueue & Queue, InputEventQueue::FilteredQueue::const_iterator InputEventIterator)
{
	auto Match = MatchDown(Queue, InputEventIterator);
	if (2 == Match.Status)
	{
		Vector2n DownPosition((*InputEventIterator)->m_PostEventState.GetAxisState(0).GetPosition(), (*InputEventIterator)->m_PostEventState.GetAxisState(1).GetPosition());
		auto DownTime = (*InputEventIterator)->GetTimestamp();
		auto PointingPointer = (*InputEventIterator)->m_Pointer;

		uint8 NumberOfTaps = 0;

		auto FilteredQueue2 = InputEventQueue::FilterByPointer(InputEventQueue::CreateFilteredQueue(Queue, Match.End), PointingPointer);
		for (auto InputEventIterator2 = FilteredQueue2.begin(); FilteredQueue2.end() != InputEventIterator2; ++InputEventIterator2)
		{
			Match.Events.push_back(*InputEventIterator2);

			if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, 0, false>(**InputEventIterator2))
			{
				Vector2n UpPosition((*InputEventIterator2)->m_PostEventState.GetAxisState(0).GetPosition(), (*InputEventIterator2)->m_PostEventState.GetAxisState(1).GetPosition());
				auto UpTime = (*InputEventIterator2)->GetTimestamp();

				if (   (UpPosition - DownPosition).LengthSquared() <= (TapRadius * TapRadius)
					&& (UpTime - DownTime) <= DoubleTapTime)
				{
					if (1 == NumberOfTaps)
					{
						++InputEventIterator2;
						return MatchResult(InputEventIterator2, Match.Events);
					}
					else
						++NumberOfTaps;
				}
				else
				{
					return MatchResult();
				}
			}
			else if (IsPointerPointingMoveEvent<0>(**InputEventIterator2))
			{
				Vector2n MovePosition((*InputEventIterator2)->m_PostEventState.GetAxisState(0).GetPosition(), (*InputEventIterator2)->m_PostEventState.GetAxisState(1).GetPosition());
				auto NewTime = (*InputEventIterator2)->GetTimestamp();

				if (   (MovePosition - DownPosition).LengthSquared() <= (TapRadius * TapRadius)
					&& (NewTime - DownTime) <= TapTime)
				{}
				else
				{
					return MatchResult();
				}
			}
			else if (IsPointerButtonEvent<Pointer::VirtualCategory::POINTING, 0, true>(**InputEventIterator2))
			{
				Vector2n DownAgainPosition((*InputEventIterator2)->m_PostEventState.GetAxisState(0).GetPosition(), (*InputEventIterator2)->m_PostEventState.GetAxisState(1).GetPosition());
				auto NewTime = (*InputEventIterator2)->GetTimestamp();

				if (   (DownAgainPosition - DownPosition).LengthSquared() <= (TapRadius * TapRadius)
					&& (NewTime - DownTime) <= TapTime)
				{}
				else
				{
					return MatchResult();
				}
			}
			else
			{
				return MatchResult();
			}
		}

		// Only if there's still a chance a new event can come in time to make a match
		if ((glfwGetTime() - DownTime) <= DoubleTapTime)
		{
			Match.Status = 1;
			return Match;
		}
	}
	else
	{
		return Match;
	}

	return MatchResult();
}

/*uint8 MatchDoubleTap3(const InputEventQueue::Queue & Queue, InputEventQueue::Queue::const_iterator & InputEventIterator)
{
	auto FirstTap = MatchTap2(Queue, InputEventIterator);
	if (2 == FirstTap)
	{
		auto SecondTap = MatchTap2(Queue, InputEventIterator);
		if (2 == SecondTap)
		{
			return 2;
		}
		else if (1 == SecondTap)
		{
			return 1;
		}
		else if (0 == SecondTap)
		{
			return 0;
		}
	}
	else if (1 == FirstTap)
	{
		return 1;
	}
	else if (0 == FirstTap)
	{
		return 0;
	}

	return 0;
}*/

GestureRecognizer::GestureRecognizer(GestureHandler & Owner)
	: InputHandler(),
	  m_Shortcuts(),
	  m_Owner(Owner)
{
}

GestureRecognizer::~GestureRecognizer()
{
}

bool GestureRecognizer::ProcessEventHandledTEST(InputEvent InputEvent)
{
	ProcessEvent(InputEvent);
	return InputEvent.m_Handled;
}

bool GestureRecognizer::ProcessShortcuts(InputEvent InputEvent)
{
	for (auto & Shortcut : m_Shortcuts)
	{
		if (   IsPointerButtonEvent<Pointer::VirtualCategory::TYPING, true>(InputEvent, Shortcut.InputId)
			&& Shortcut.Modifiers == InputEvent.m_PostEventState.GetModifiers())
		{
			Shortcut.Action();

			return true;
		}
	}

	return false;
}

MatchResult GestureRecognizer::MatchEventQueue(InputEventQueue::FilteredQueue & UnreservedEvents)
{
	auto InputEventIterator = UnreservedEvents.begin();
	auto & InputEvent = **InputEventIterator;

#if 1
	// If the pointer is not connected to this GR (meaning a failed HitTest), return failed match
	// DEBUG: Is this the right way to go about it? Or a temporary hack? Figure it out.
	if (   nullptr != InputEvent.m_Pointer
		&& GetConnected().end() == GetConnected().find(InputEvent.m_Pointer)
		&& nullptr == dynamic_cast<TypingModule *>(&m_Owner))		// HACK!! (To enable TypingModule to work, i.e. get input while not being pointed to)
	{
		m_InManipulation = false;		// HACK: Not sure if this is the best way of doing it

		return MatchResult();
	}
#else
	Vector2n GlobalPosition(InputEvent.m_PreEventState.GetAxisState(0).GetPosition(), InputEvent.m_PreEventState.GetAxisState(1).GetPosition());
	//printf("Global Pos %d, %d.\n", GlobalPosition.X(), GlobalPosition.Y());
	if (/*   !m_InManipulation
		&& */!static_cast<Widget &>(m_Owner).IsHit(static_cast<Widget &>(m_Owner).GlobalToParent(GlobalPosition))
		&& (   nullptr != InputEvent.m_Pointer
			&& GetConnected().end() == GetConnected().find(InputEvent.m_Pointer)))
	{
		return MatchResult();
	}
#endif

// DEBUG: I don't think the following is neccessary anymore, so I will try just setting Hit to true at all times and see if that breaks anything. If not, might as well remove this (unneccessary?) check
#if 0
	Vector2n GlobalPosition(InputEvent.m_PreEventState.GetAxisState(0).GetPosition(), InputEvent.m_PreEventState.GetAxisState(1).GetPosition());
	auto Hit = static_cast<Widget &>(m_Owner).IsHit(static_cast<Widget &>(m_Owner).GlobalToParent(GlobalPosition));
	/*if (!Hit) {
		std::cout << "OMGGG LOOK HERE!!!! Hit IS false\n";
	}*/
#else
	auto Hit = true;
#endif

	MatchResult Match;
	if (m_RecognizeDoubleTap && (Match = MatchDoubleTap2(UnreservedEvents, InputEventIterator)).AnySuccess())
	{
		if (2 == Match.Status)
		{
			m_Owner.ProcessDoubleTap(InputEvent, Vector2n((*InputEventIterator)->m_PostEventState.GetAxisState(0).GetPosition(), (*InputEventIterator)->m_PostEventState.GetAxisState(1).GetPosition()));
		}
	}
	else if (m_RecognizeTap && (Match = MatchTap2(UnreservedEvents, InputEventIterator)).AnySuccess())
	{
		if (2 == Match.Status)
		{
			m_Owner.ProcessTap(InputEvent, Vector2n((*InputEventIterator)->m_PostEventState.GetAxisState(0).GetPosition(), (*InputEventIterator)->m_PostEventState.GetAxisState(1).GetPosition()));
		}
	}
	else if (m_RecognizeManipulationTranslate && (Match = MatchManipulationBegin(UnreservedEvents, InputEventIterator, m_InManipulation, Hit, m_RecognizeManipulationTranslateButtonId)).AnySuccess())
	{
		if (2 == Match.Status)
		{
			m_InManipulation = true;
			m_Owner.ProcessManipulationBegin(InputEvent);
		}
	}
	else if (m_RecognizeManipulationTranslate && (Match = MatchManipulationUpdate(UnreservedEvents, InputEventIterator, m_InManipulation)).AnySuccess())
	{
		if (2 == Match.Status)
		{
			m_Owner.ProcessManipulationUpdate(InputEvent);
		}
	}
	else if (m_RecognizeManipulationTranslate && (Match = MatchManipulationEnd(UnreservedEvents, InputEventIterator, m_InManipulation, m_RecognizeManipulationTranslateButtonId)).AnySuccess())
	{
		if (2 == Match.Status)
		{
			m_Owner.ProcessManipulationEnd(InputEvent);
			m_InManipulation = false;
		}
	}
	else if (ProcessShortcuts(InputEvent))
	{
		Match.Status = 2;
		Match.Events.push_back(*InputEventIterator);
	}
	else if (ProcessEventHandledTEST(InputEvent))
	{
		Match.Status = 2;
		Match.Events.push_back(*InputEventIterator);
	}

	return Match;
}

void GestureRecognizer::ProcessEvent(InputEvent & InputEvent)
{
	// DEBUG: This check is probably not needed anymore... it should always pass, due to new architecture
	if (true == InputEvent.m_Handled)
	{
		printf("Exception in void GestureRecognizer::ProcessEvent(InputEvent & InputEvent), InputEvent.m_Handled is not false!");
		throw 0;
		return;
	}

	// DECISION
/*#if 1
	// TEST: Capture the pointer if the pointer is activated (via primary button)
	if (   InputEvent.HasType(InputEvent::EventType::POINTER_ACTIVATION)
		&& (   InputEvent.HasType(InputEvent::EventType::BUTTON_EVENT)
			&& 0 == InputEvent.m_InputId
			&& true == InputEvent.m_Buttons[0]))
	{
		InputEvent.m_Pointer->ModifyPointerMapping().RequestPointerCapture(this);
	}

	// Release the pointer capture if the pointer is deactivated
	if (InputEvent.HasType(InputEvent::EventType::POINTER_DEACTIVATION))
	{
		InputEvent.m_Pointer->ModifyPointerMapping().RequestPointerRelease(this);
	}
#endif*/

	// EXPERIMENTAL
	/*if (   m_RecognizeDoubleTap
		&& !InputEvent.m_Handled)
	{
		if (   InputEvent.HasType(InputEvent::EventType::BUTTON_EVENT)
			&& 0 == InputEvent.m_InputId
			&& false == InputEvent.m_Buttons[0]
			&& std::fabs(InputEvent.m_Pointer->GetPointerState().GetTimestamp() - m_LastTapCompletedStateTEST.GetTimestamp()) <= 0.400
			&& (Vector2n(InputEvent.m_Pointer->GetPointerState().GetAxisState(0).GetPosition(), InputEvent.m_Pointer->GetPointerState().GetAxisState(1).GetPosition()) - Vector2n(m_LastTapStateTEST.GetAxisState(0).GetPosition(), m_LastTapStateTEST.GetAxisState(1).GetPosition())).LengthSquared() <= (3 * 3))
		{
			printf("Recognized a double tap of %f ms.\n", std::fabs(InputEvent.m_Pointer->GetPointerState().GetTimestamp() - m_LastTapCompletedStateTEST.GetTimestamp()) * 1000);
			InputEvent.m_Handled = true;
			m_Owner.ProcessDoubleTap(InputEvent, Vector2n(m_LastTapStateTEST.GetAxisState(0).GetPosition(), m_LastTapStateTEST.GetAxisState(1).GetPosition()));
			m_LastTapCompletedStateTEST.InvalidateTEST();
		}
	}*/

	// TODO: Fix bug where dragging the object out of down-event range and then bringing it back still activates the tap (it shouldn't)
	/*if (   m_RecognizeTap
		&& !InputEvent.m_Handled)
	{
		if (   InputEvent.HasType(InputEvent::EventType::BUTTON_EVENT)
			&& 0 == InputEvent.m_InputId
			&& true == InputEvent.m_Buttons[0])
		{
			m_LastTapStateTEST = InputEvent.m_Pointer->GetPointerState();
		}
		else if (   InputEvent.HasType(InputEvent::EventType::BUTTON_EVENT)
				 && 0 == InputEvent.m_InputId
				 && false == InputEvent.m_Buttons[0]
				 //&& std::fabs(InputEvent.m_Timestamp - m_LastTapEventTEST.m_Timestamp) <= 1.0)
				 && (Vector2n(InputEvent.m_Pointer->GetPointerState().GetAxisState(0).GetPosition(), InputEvent.m_Pointer->GetPointerState().GetAxisState(1).GetPosition()) - Vector2n(m_LastTapStateTEST.GetAxisState(0).GetPosition(), m_LastTapStateTEST.GetAxisState(1).GetPosition())).LengthSquared() <= (3 * 3))
		{
			printf("Recognized a tap.\n");
			InputEvent.m_Handled = true;
			m_Owner.ProcessTap(InputEvent, Vector2n(m_LastTapStateTEST.GetAxisState(0).GetPosition(), m_LastTapStateTEST.GetAxisState(1).GetPosition()));
			m_LastTapCompletedStateTEST = m_LastTapStateTEST;
		}
	}*/

	//if (m_RecognizeDrag)
	/*{
		if (   InputEvent.HasType(InputEvent::EventType::AXIS_EVENT)
			&& 0 == InputEvent.m_InputId
			&& true == InputEvent.GetPointerState().GetButtonState(0))
		{
			//printf("Recognized a drag by (%d, %d).\n", InputEvent.m_Sliders[0], InputEvent.m_Sliders[1]);

			//double Scaler = GetAttached().size();
			//printf("GetAttached().size() %d\n", GetAttached().size());

			//m_Owner.ProcessDrag(Vector2d(InputEvent.m_Sliders[0] / Scaler, InputEvent.m_Sliders[1] / Scaler));
			m_Owner.ProcessDrag(Vector2d(InputEvent.m_Sliders[0], InputEvent.m_Sliders[1]));		// TODO: Figure out this GetAttached() stuff
		}
	}*/

	if (   m_RecognizeScroll
		&& !InputEvent.m_Handled)
	{
		if (   InputEvent.HasType(InputEvent::EventType::AXIS_EVENT)
			&& 2 == InputEvent.m_InputId)
		{
			//printf("Recognized a wheel move by %d.\n", InputEvent.m_Sliders[0]);
			m_Owner.ProcessScroll(InputEvent, Vector2n(InputEvent.m_Sliders[0], InputEvent.m_Sliders[1]));
			InputEvent.m_Handled = true;
		}
	}

	// TODO: Support for manipulation with >1 pointer simultaneously (including translation, as well as rotation/scale)
	/*if (   m_RecognizeManipulationTranslate
		&& !InputEvent.m_Handled)
	{
		if (   InputEvent.HasType(InputEvent::EventType::BUTTON_EVENT)
			&& 0 == InputEvent.m_InputId
			&& true == InputEvent.m_Buttons[0])
		{
			m_InManipulation = true;
			m_Owner.ProcessManipulationBegin(InputEvent.m_Pointer->GetPointerState());
			// DECISION
			//InputEvent.m_Pointer->ModifyPointerMapping().RequestPointerCapture(this);		// TEST
			InputEvent.m_Handled = true;
		}

		if (   InputEvent.HasType(InputEvent::EventType::AXIS_EVENT)
			&& 0 == InputEvent.m_InputId
			&& true == InputEvent.m_Pointer->GetPointerState().GetButtonState(0))
		{
			//double Scaler = GetAttached().size();

			m_Owner.ProcessManipulationUpdate(InputEvent.m_Pointer->GetPointerState());
			// DECISION
			//InputEvent.m_Pointer->ModifyPointerMapping().RequestPointerCapture(this);		// TEST
			InputEvent.m_Handled = true;
		}

		if (   InputEvent.HasType(InputEvent::EventType::BUTTON_EVENT)
			&& 0 == InputEvent.m_InputId
			&& false == InputEvent.m_Buttons[0])
		{
			m_InManipulation = false;
			m_Owner.ProcessManipulationEnd(InputEvent.m_Pointer->GetPointerState());
			// DECISION
			//InputEvent.m_Pointer->ModifyPointerMapping().RequestPointerRelease(this);		// TEST
			InputEvent.m_Handled = true;
		}
	}*/

	//if (   m_RecognizeCharacters
	if (   true
		&& !InputEvent.m_Handled)
	{
		if (InputEvent.HasType(InputEvent::EventType::CHARACTER_EVENT))
		{
			m_Owner.ProcessCharacter(InputEvent, InputEvent.m_InputId);
			//InputEvent.m_Handled = true;
		}
	}

	// TODO: There might be duplication here this way, I think I should eliminate it (by providing a complete alternative gesture-level api for all events
	// Low-level event passthrough
	if (!InputEvent.m_Handled)
	{
		m_Owner.ProcessEvent(InputEvent);
	}
}

void GestureRecognizer::ProcessCanvasUpdated()
{
	if (   m_InManipulation
		&& !GetConnected().empty())
	{
		// HACK: Using the first pointer is a hack
		// TODO: I should pass all connected pointers as a 'frame', they're all relevant
		// HACK: Disabled because this commit changed signature for ProcessManipulationUpdate() and I don't have access to InputEvent here...
		//m_Owner.ProcessManipulationUpdate((*GetConnected().begin())->GetPointerState());
	}
}

GestureHandler & GestureRecognizer::GetOwner()
{
	return m_Owner;
}

void GestureRecognizer::AddShortcut(ShortcutEntry ShortcutEntry)
{
	m_Shortcuts.push_back(ShortcutEntry);
}

void GestureRecognizer::RemoveShortcut(ShortcutEntry::Key Key)
{
	for (auto it0 = m_Shortcuts.begin(); it0 != m_Shortcuts.end(); ++it0)
	{
		if (   it0->InputId == Key.first
			&& it0->Modifiers == Key.second)
		{
			m_Shortcuts.erase(it0);
			break;
		}
	}
}
