/***************************************************************************
                          TrackPanelTransitionResizeFunction.cpp  -  description
                             -------------------
    begin                : Sun Feb 9 2006
    copyright            : (C) 2006 Jean-Baptiste Mardelle, jb@ader.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "trackpaneltransitionresizefunction.h"

#include "kdebug.h"

#include "doctrackbase.h"
#include "kdenlive.h"
#include "ktimeline.h"
#include "kresizecommand.h"
#include "kselectclipcommand.h"
#include "docclipproject.h"
#include "transitionstack.h"

#include <cmath>

// static
const uint TrackPanelTransitionResizeFunction::s_resizeTolerance = 5;

TrackPanelTransitionResizeFunction::TrackPanelTransitionResizeFunction(Gui::KdenliveApp * app, Gui::KTimeLine * timeline, KdenliveDoc * document):
m_app(app),
m_timeline(timeline),
m_document(document),
m_clipUnderMouse(0),
m_resizeCommand(0),
m_selectedTransition(0),
m_snapToGrid(), m_refresh(false)
{
    
}

TrackPanelTransitionResizeFunction::~TrackPanelTransitionResizeFunction()
{
}


bool TrackPanelTransitionResizeFunction::mouseApplies(Gui::KTrackPanel * panel,
    QMouseEvent * event) const
{
    bool result = false;

    if (panel->hasDocumentTrackIndex()) {
	DocTrackBase *track =
	    m_document->track(panel->documentTrackIndex());
	if (track) {
	    GenTime mouseTime(m_timeline->mapLocalToValue(event->x()),
		m_document->framesPerSecond());
	    DocClipRef *clip = track->getClipAt(mouseTime);
	    if (clip) {
                TransitionStack m_transitions = clip->clipTransitions();
                if (m_transitions.isEmpty()) return false;

                TransitionStack::iterator itt = m_transitions.begin();
                //  Loop through the clip's transitions
                while (itt) {
                    uint dx1 = (*itt)->transitionStartTime().frames(m_document->framesPerSecond());
                    uint dx2 = (*itt)->transitionEndTime().frames(m_document->framesPerSecond());
			if ((fabs(m_timeline->mapValueToLocal(dx1) - event->x()) < s_resizeTolerance))
                            return true;
                        if ((fabs(m_timeline->mapValueToLocal(dx2) - event->x()) < s_resizeTolerance))
                            return true;
                ++itt;
                }
	    }
	}
    }
    return result;
}


QCursor TrackPanelTransitionResizeFunction::getMouseCursor(Gui::KTrackPanel *
    panel, QMouseEvent * event)
{
//return QCursor(Qt::SizeVerCursor);
    return QCursor(Qt::SizeHorCursor);
}

bool TrackPanelTransitionResizeFunction::mousePressed(Gui::KTrackPanel * panel,
    QMouseEvent * event)
{
    bool result = false;

    if (panel->hasDocumentTrackIndex()) {
	DocTrackBase *track =
	    m_document->track(panel->documentTrackIndex());
	if (track) {
	    GenTime mouseTime(m_timeline->mapLocalToValue(event->x()),
		m_document->framesPerSecond());
	    m_clipUnderMouse = track->getClipAt(mouseTime);
	    if (m_clipUnderMouse) {
                
                TransitionStack m_transitions = m_clipUnderMouse->clipTransitions();
                if (m_transitions.isEmpty()) return false;

                TransitionStack::iterator itt = m_transitions.begin();
                //  Loop through the clip's transitions
                
                int dx1;
                int dx2;
                
                m_resizeState = None;
                while (itt) {
                dx1 = (*itt)->transitionStartTime().frames(m_document->framesPerSecond());
                dx2 = (*itt)->transitionEndTime().frames(m_document->framesPerSecond());
                    if ((fabs(m_timeline->mapValueToLocal(dx1) - event->x()) < s_resizeTolerance)) {
                        m_resizeState = Start;
                        break;
                    }
                    if ((fabs(m_timeline->mapValueToLocal(dx2) - event->x()) < s_resizeTolerance)) {
                        m_resizeState = End;
                        break;
                    }
                    ++itt;

                }
                if (m_resizeState == None ) return false;
                m_selectedTransition = (*itt)->clone();
                        m_app->
		    addCommand(Command::KSelectClipCommand::
                                selectNone(m_document), true);

                        m_app->
		    addCommand(Command::KSelectClipCommand::
                                selectClipAt(m_document, *track,
                                             (m_clipUnderMouse->trackStart() +
                                                     m_clipUnderMouse->trackEnd()) / 2.0));

                        m_snapToGrid.clearSnapList();
                        if (m_timeline->snapToSeekTime())
                            m_snapToGrid.addToSnapList(m_timeline->seekPosition());
                        m_snapToGrid.setSnapToFrame(m_timeline->snapToFrame());

                        m_snapToGrid.addToSnapList(m_document->
                                getSnapTimes(m_timeline->snapToBorders(),
                                             m_timeline->snapToMarkers(), true, false));

                        m_snapToGrid.setSnapTolerance(GenTime(m_timeline->
                                mapLocalToValue(Gui::KTimeLine::snapTolerance) -
                                m_timeline->mapLocalToValue(0),
                        m_document->framesPerSecond()));

                        QValueVector < GenTime > cursor;

                        if (m_resizeState == Start) {
                            cursor.append((*itt)->transitionStartTime());
                        } else if (m_resizeState == End) {
                            cursor.append((*itt)->transitionEndTime());
                        }
                        m_snapToGrid.setCursorTimes(cursor);
                        /*m_resizeCommand =
                                new Command::KResizeCommand(m_document,
                                *m_clipUnderMouse);*/

                        result = true;
            }
        }
    }

    return result;
}

bool TrackPanelTransitionResizeFunction::mouseReleased(Gui::KTrackPanel * panel,
    QMouseEvent * event)
{
    bool result = false;
    m_selectedTransition = 0;
    emit transitionChanged(true);
    // Select the keyframe
    /*
    Effect *effect = m_clipUnderMouse->selectedEffect();
    uint effectIndex = 0;
    effect->parameter(effectIndex)->
	setSelectedKeyFrame(m_selectedKeyframe);
    */
    //emit redrawTrack();

/*	m_resizeCommand->setEndSize(*m_clipUnderMouse);
	m_app->addCommand(m_resizeCommand, false);
	m_document->indirectlyModified();
	m_resizeCommand = 0;*/

    result = true;
    return result;
}


bool TrackPanelTransitionResizeFunction::mouseMoved(Gui::KTrackPanel * panel,
    QMouseEvent * event)
{
    bool result = false;    
    if (panel->hasDocumentTrackIndex()) {
	DocTrackBase *track =
	    m_document->track(panel->documentTrackIndex());
	if (track) {
	    GenTime mouseTime =
		m_snapToGrid.getSnappedTime(m_timeline->
		timeUnderMouse(event->x()));

            if (m_clipUnderMouse && m_selectedTransition) {
                result = true;
                if (m_resizeState == Start) {
                    m_clipUnderMouse->resizeTransitionStart(m_selectedTransition, mouseTime);
                    //emit signalClipCropStartChanged(m_clipUnderMouse);
                } else if (m_resizeState == End) {
                    m_clipUnderMouse->resizeTransitionEnd(m_selectedTransition, mouseTime);
                    //emit signalClipCropEndChanged(m_clipUnderMouse);
                } else {
                    kdError() <<
                            "Unknown resize state reached in KMMTimeLineTrackView::mouseMoveEvent()"
                            << endl;
                    kdError() << "(this message should never be seen!)" <<
                            endl;
                }
            }
	}
    }

    return result;
}
