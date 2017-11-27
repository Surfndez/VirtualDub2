/**
 * Debugmode Frameserver
 * Copyright (C) 2002-2009 Satish Kumar, All Rights Reserved
 * http://www.debugmode.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * http://www.gnu.org/copyleft/gpl.html
 */

#ifndef DFSC_DFSC_H
#define DFSC_DFSC_H

#include <windows.h>
#include <vfw.h>
#include <vd2/riza/videocodec.h>
#include <vd2/riza/avi.h>
#include <vd2/kasumi/pixmap.h>

static const DWORD FOURCC_DFSC = mmioFOURCC('D', 'F', 'S', 'C');
#define WAVE_FORMAT_DFSC 0xDFAC

#pragma pack(4)

typedef struct {
  char videoEncSemName[64],
       videoEncEventName[64],
       videoDecEventName[64],
       audioEncSemName[64],
       audioEncEventName[64],
       audioDecEventName[64];
  char signpostPath[MAX_PATH * 2];
  int encStatus, decStatus;
  DWORD videoFrameIndex, videoBytesRead;
  DWORD audioFrameIndex, audioBytesRead;
  BOOL audioRequestFullSecond;
  int numNetworkClientsConnected;
  DWORD totalAVDataSize;
  BITMAPINFOHEADER encBi;
  WAVEFORMATEX wfx;
  DWORD videooffset;
  DWORD audiooffset;
} DfscData;

#pragma pack()

class DfscInstance;

class VDVideoDecompressorDFSC : public IVDVideoDecompressor {
public:
  VDVideoDecompressorDFSC();
  ~VDVideoDecompressorDFSC();

  void Init(const void *srcFormat, uint32 srcFormatSize);

  bool QueryTargetFormat(int format);
  bool QueryTargetFormat(const void *format);
  bool SetTargetFormat(int format);
  bool SetTargetFormat(const void *format);
  int GetTargetFormat() { return mFormat; }
  int GetTargetFormatVariant() { return 0; }
  const uint32 *GetTargetFormatPalette() { return NULL; }
  void Start();
  void Stop();
  void DecompressFrame(void *dst, const void *src, uint32 srcSize, bool keyframe, bool preroll);
  const void *GetRawCodecHandlePtr();
  const wchar_t *GetName();
  bool GetAlpha(){ return mFormat==nsVDPixmap::kPixFormat_XRGB8888; }

protected:
  int		mFormat;
  DfscInstance* pinst;
  vdstructex<VDAVIBitmapInfoHeader>	mSrcFormat;
  vdstructex<VDAVIBitmapInfoHeader>	mDstFormat;
};

#endif
