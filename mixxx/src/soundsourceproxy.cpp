/* -*- mode:C++; indent-tabs-mode:t; tab-width:8; c-basic-offset:4; -*- */
/***************************************************************************
                          soundsourceproxy.cpp  -  description
                             -------------------
    begin                : Wed Oct 13 2004
    copyright            : (C) 2004 Tue Haste Andersen
    email                :
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "trackinfoobject.h"
#include "soundsourceproxy.h"
#include "soundsourcemp3.h"
#include "soundsourceoggvorbis.h"
#ifdef __SNDFILE__
#include "soundsourcesndfile.h"
#endif
#ifdef __AUDIOFILE__
#include "soundsourceaudiofile.h"
#endif
#ifdef __FFMPEGFILE__
#include "soundsourceffmpeg.h"
#endif

//Added by qt3to4:
#include <Q3ValueList>


SoundSourceProxy::SoundSourceProxy(QString qFilename) : SoundSource(qFilename)
{
#ifdef __FFMPEGFILE__
    m_pSoundSource = new SoundSourceFFmpeg(qFilename);
    return;
#endif
    if (qFilename.lower().endsWith(".mp3"))
        m_pSoundSource = new SoundSourceMp3(qFilename);
    else if (qFilename.lower().endsWith(".ogg"))
        m_pSoundSource = new SoundSourceOggVorbis(qFilename);
    else
#ifdef __SNDFILE__
        m_pSoundSource = new SoundSourceSndFile(qFilename);
#endif
#ifdef __AUDIOFILE__
        m_pSoundSource = new SoundSourceAudioFile(qFilename);
#endif
}

SoundSourceProxy::SoundSourceProxy(TrackInfoObject * pTrack) : SoundSource(pTrack->getLocation())
{
    QString qFilename = pTrack->getLocation();

#ifdef __FFMPEGFILE__
    m_pSoundSource = new SoundSourceFFmpeg(qFilename);
    return;
#endif

    if (qFilename.lower().endsWith(".mp3"))
	m_pSoundSource = new SoundSourceMp3(qFilename);
    else if (qFilename.lower().endsWith(".ogg"))
	m_pSoundSource = new SoundSourceOggVorbis(qFilename);
    else
    {
#ifdef __SNDFILE__
	m_pSoundSource = new SoundSourceSndFile(qFilename);
#endif
#ifdef __AUDIOFILE__
	m_pSoundSource = new SoundSourceAudioFile(qFilename);
#endif
    }

    pTrack->setDuration(length()/(2*getSrate()));
}

SoundSourceProxy::~SoundSourceProxy()
{
    delete m_pSoundSource;
}

long SoundSourceProxy::seek(long l)
{
    return m_pSoundSource->seek(l);
}

unsigned SoundSourceProxy::read(unsigned long size, const SAMPLE * p)
{
    return m_pSoundSource->read(size, p);
}

long unsigned SoundSourceProxy::length()
{
    return m_pSoundSource->length();
}

int SoundSourceProxy::ParseHeader(TrackInfoObject * p)
{
    QString qFilename = p->getFilename();
#ifdef __FFMPEGFILE__
    return SoundSourceFFmpeg::ParseHeader(p);;
#endif
    if (qFilename.lower().endsWith(".mp3"))
	return SoundSourceMp3::ParseHeader(p);
    else if (qFilename.lower().endsWith(".ogg"))
	return SoundSourceOggVorbis::ParseHeader(p);
    else if (qFilename.lower().endsWith(".wav") || qFilename.lower().endsWith(".aif") ||
	     qFilename.lower().endsWith(".aiff") || qFilename.lower().endsWith(".flac"))
#ifdef __SNDFILE__
	return SoundSourceSndFile::ParseHeader(p);
#endif
#ifdef __AUDIOFILE__
    return SoundSourceAudioFile::ParseHeader(p);
#endif

    return ERR;
}


int SoundSourceProxy::getSrate()
{
    return m_pSoundSource->getSrate();
}

Q3ValueList<long> * SoundSourceProxy::getCuePoints()
{
    return m_pSoundSource->getCuePoints();
}

QString SoundSourceProxy::getFilename()
{
    return m_pSoundSource->getFilename();
}
