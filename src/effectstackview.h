/***************************************************************************
                          effecstackview.h  -  description
                             -------------------
    begin                : Feb 15 2008
    copyright            : (C) 2008 by Marco Gittler
    email                : g.marco@freenet.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EFFECTSTACKVIEW_H
#define EFFECTSTACKVIEW_H

#include "ui_effectstack_ui.h"
#include "effectstackedit.h"

class EffectsList;
class ClipItem;
class MltVideoProfile;

class EffectStackView : public QWidget
{
    Q_OBJECT

public:
    EffectStackView(QWidget *parent = 0);
    virtual ~EffectStackView();
    void raiseWindow(QWidget*);
    void clear();
    void setMenu(QMenu *menu);
    void updateProjectFormat(MltVideoProfile profile, Timecode t);

private:
    Ui::EffectStack_UI m_ui;
    ClipItem* m_clipref;
    QMap<QString, EffectsList*> m_effectLists;
    EffectStackEdit* m_effectedit;
    void setupListView(int ix);
    //void updateButtonStatus();

public slots:
    void slotClipItemSelected(ClipItem*, int ix);
    void slotUpdateEffectParams(const QDomElement, const QDomElement);

private slots:
    void slotItemSelectionChanged(bool update = true);
    void slotItemUp();
    void slotItemDown();
    void slotItemDel();
    void slotResetEffect();
    void slotItemChanged(QListWidgetItem *item);
    void slotSaveEffect();

signals:
    void removeEffect(ClipItem*, QDomElement);
    /**  Parameters for an effect changed, update the filter in playlist */
    void updateClipEffect(ClipItem*, QDomElement, QDomElement, int);
    /** An effect in stack was moved, we need to regenerate
        all effects for this clip in the playlist */
    void refreshEffectStack(ClipItem *);
    /** Enable or disable an effect */
    void changeEffectState(ClipItem*, int, bool);
    /** An effect in stack was moved */
    void changeEffectPosition(ClipItem*, int, int);
    /** an effect was saved, reload list */
    void reloadEffects();

};

#endif
