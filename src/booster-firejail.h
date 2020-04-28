/***************************************************************************
**
** Copyright (c) 2020 Open Mobile Platform LLC.
** All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef BOOSTER_FIREJAIL_H
#define BOOSTER_FIREJAIL_H

#include "booster.h"

class FirejailBooster : public Booster
{
public:

    FirejailBooster() {}
    virtual ~FirejailBooster() {}

    //! \reimp
    virtual const string & boosterType() const;

protected:

    //! \reimp
    virtual int launchProcess();

    //! \reimp
    virtual bool preload();

private:

    //! Disable copy-constructor
    FirejailBooster(const FirejailBooster & r);

    //! Disable assignment operator
    FirejailBooster & operator= (const FirejailBooster & r);

    static const string m_boosterType;

    //! wait for socket connection
    void accept();

};

#endif
