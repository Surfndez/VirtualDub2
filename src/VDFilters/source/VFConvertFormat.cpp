//	VirtualDub - Video processing and capture application
//	Copyright (C) 1998-2007 Avery Lee
//
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "stdafx.h"
#include <windows.h>
#include "resource.h"
#include <vd2/system/strutil.h>
#include <vd2/VDLib/Dialog.h>
#include <vd2/VDXFrame/VideoFilter.h>
#include <vd2/Kasumi/pixmaputils.h>

class VDVFilterConvertFormat;

class VDVFilterConvertFormatConfigDialog : public VDDialogFrameW32 {
public:
	VDVFilterConvertFormatConfigDialog(VDPixmapFormatEx format);

	bool OnLoaded();
	bool OnCommand(uint32 id, uint32 extcode);
	void OnDataExchange(bool write);
	void InitFocus();
	void SyncControls();
	void SyncInputColor();
	void redo();

	VDPixmapFormatEx mFormat;
	IVDXFilterPreview2 *fp;
	VDVFilterConvertFormat* filter;

	struct FormatButtonMapping {
		int mFormat;
		uint32 mInputButton;
	};

	static const FormatButtonMapping kFormatButtonMappings[];
};

const VDVFilterConvertFormatConfigDialog::FormatButtonMapping VDVFilterConvertFormatConfigDialog::kFormatButtonMappings[] = {
	{	nsVDPixmap::kPixFormat_Null,			IDC_INPUT_AUTOSELECT},
	{	nsVDPixmap::kPixFormat_XRGB8888,		IDC_INPUT_XRGB8888},
	{	nsVDPixmap::kPixFormat_XRGB64,			IDC_INPUT_XRGB64},
	{	nsVDPixmap::kPixFormat_YUV422_UYVY,		IDC_INPUT_YUV422_UYVY},
	{	nsVDPixmap::kPixFormat_YUV422_YUYV,		IDC_INPUT_YUV422_YUY2},
	{	nsVDPixmap::kPixFormat_YUV420_Planar,	IDC_INPUT_YUV420_PLANAR},
	{	nsVDPixmap::kPixFormat_YUV422_Planar,	IDC_INPUT_YUV422_PLANAR},
	{	nsVDPixmap::kPixFormat_YUV411_Planar,	IDC_INPUT_YUV411_PLANAR},
	{	nsVDPixmap::kPixFormat_YUV410_Planar,	IDC_INPUT_YUV410_PLANAR},
	{	nsVDPixmap::kPixFormat_YUV444_Planar,	IDC_INPUT_YUV444_PLANAR},
	{	nsVDPixmap::kPixFormat_YUV444_Planar16,	IDC_INPUT_YUV444_PLANAR16},
	{	nsVDPixmap::kPixFormat_YUV422_Planar16,	IDC_INPUT_YUV422_PLANAR16},
	{	nsVDPixmap::kPixFormat_YUV420_Planar16,	IDC_INPUT_YUV420_PLANAR16},
};

VDVFilterConvertFormatConfigDialog::VDVFilterConvertFormatConfigDialog(VDPixmapFormatEx format)
	: VDDialogFrameW32(IDD_FILTER_CONVERTFORMAT)
	, mFormat(format)
{
	fp = 0;
	filter = 0;
}

bool VDVFilterConvertFormatConfigDialog::OnLoaded() {
	OnDataExchange(false);
	InitFocus();
	VDDialogFrameW32::OnLoaded();
	if (fp) {
		EnableWindow(GetDlgItem(mhdlg, IDC_PREVIEW), TRUE);
		fp->InitButton((VDXHWND)GetDlgItem(mhdlg, IDC_PREVIEW));
	}
	return true;
}

void VDVFilterConvertFormatConfigDialog::OnDataExchange(bool write) {
	if (write) {
	} else {
		SyncControls();
	}
}

bool VDVFilterConvertFormatConfigDialog::OnCommand(uint32 id, uint32 extcode) {
	if (extcode == BN_CLICKED) {
		switch(id) {
			case IDC_CS_NONE:
				mFormat.colorSpaceMode = nsVDXPixmap::kColorSpaceMode_None;
				redo();
				return TRUE;

			case IDC_CS_601:
				mFormat.colorSpaceMode = nsVDXPixmap::kColorSpaceMode_601;
				redo();
				return TRUE;

			case IDC_CS_709:
				mFormat.colorSpaceMode = nsVDXPixmap::kColorSpaceMode_709;
				redo();
				return TRUE;

			case IDC_CR_NONE:
				mFormat.colorRangeMode = nsVDXPixmap::kColorRangeMode_None;
				redo();
				return TRUE;

			case IDC_CR_LIMITED:
				mFormat.colorRangeMode = nsVDXPixmap::kColorRangeMode_Limited;
				redo();
				return TRUE;

			case IDC_CR_FULL:
				mFormat.colorRangeMode = nsVDXPixmap::kColorRangeMode_Full;
				redo();
				return TRUE;

			case IDC_PREVIEW:
				if (fp) fp->Toggle((VDXHWND)mhdlg);
				return TRUE;
		}

		for(int i=0; i<(int)sizeof(kFormatButtonMappings)/sizeof(kFormatButtonMappings[0]); ++i) {
			const FormatButtonMapping& fbm = kFormatButtonMappings[i];
			if (fbm.mInputButton == id) {
				mFormat.format = fbm.mFormat;
				SyncInputColor();
				redo();
			}
		}
	}

	return false;
}

void VDVFilterConvertFormatConfigDialog::InitFocus() {
	for(int i=0; i<(int)sizeof(kFormatButtonMappings)/sizeof(kFormatButtonMappings[0]); ++i) {
		const FormatButtonMapping& fbm = kFormatButtonMappings[i];

		if (fbm.mFormat == mFormat) {
			SetFocusToControl(fbm.mInputButton);
			return;
		}
	}

	SetFocusToControl(IDC_INPUT_AUTOSELECT);
}

void VDVFilterConvertFormatConfigDialog::SyncControls() {
	uint32 inputButton = IDC_INPUT_AUTOSELECT;
	for(int i=0; i<(int)sizeof(kFormatButtonMappings)/sizeof(kFormatButtonMappings[0]); ++i) {
		const FormatButtonMapping& fbm = kFormatButtonMappings[i];

		if (fbm.mFormat == mFormat)
			inputButton = fbm.mInputButton;
	}

	CheckButton(inputButton, true);
	SyncInputColor();
}

void VDVFilterConvertFormatConfigDialog::SyncInputColor() {
	bool enable = VDPixmapFormatMatrixType(mFormat)!=0;
	if (mFormat==0) enable = true;

	EnableControl(IDC_STATIC_COLORSPACE, enable);
	EnableControl(IDC_STATIC_COLORRANGE, enable);
	EnableControl(IDC_CS_NONE,   enable);
	EnableControl(IDC_CS_601,    enable);
	EnableControl(IDC_CS_709,    enable);
	EnableControl(IDC_CR_NONE,   enable);
	EnableControl(IDC_CR_LIMITED,enable);
	EnableControl(IDC_CR_FULL,   enable);
	if (enable) {
		CheckButton(IDC_CS_NONE, mFormat.colorSpaceMode == nsVDXPixmap::kColorSpaceMode_None);
		CheckButton(IDC_CS_601, mFormat.colorSpaceMode == nsVDXPixmap::kColorSpaceMode_601);
		CheckButton(IDC_CS_709, mFormat.colorSpaceMode == nsVDXPixmap::kColorSpaceMode_709);
		CheckButton(IDC_CR_NONE, mFormat.colorRangeMode == nsVDXPixmap::kColorRangeMode_None);
		CheckButton(IDC_CR_LIMITED, mFormat.colorRangeMode == nsVDXPixmap::kColorRangeMode_Limited);
		CheckButton(IDC_CR_FULL, mFormat.colorRangeMode == nsVDXPixmap::kColorRangeMode_Full);
	} else {
		CheckButton(IDC_CS_NONE,    true);
		CheckButton(IDC_CS_601,     false);
		CheckButton(IDC_CS_709,     false);
		CheckButton(IDC_CR_NONE,    true);
		CheckButton(IDC_CR_LIMITED, false);
		CheckButton(IDC_CR_FULL,    false);
	}
}

///////////////////////////////////////////////////////////////////////////////

class VDVFilterConvertFormat : public VDXVideoFilter {
public:
	VDVFilterConvertFormat();

	uint32 GetParams();
	void Run();

	bool Configure(VDXHWND hwnd);

	void GetSettingString(char *buf, int maxlen);
	void GetScriptString(char *buf, int maxlen);

	void ScriptConfig(IVDXScriptInterpreter *, const VDXScriptValue *argv, int argc);
	void ScriptConfig3(IVDXScriptInterpreter *, const VDXScriptValue *argv, int argc);

	VDXVF_DECLARE_SCRIPT_METHODS();

	VDPixmapFormatEx mFormat;
};

VDVFilterConvertFormat::VDVFilterConvertFormat()
	: mFormat(nsVDXPixmap::kPixFormat_XRGB8888)
{
}

uint32 VDVFilterConvertFormat::GetParams() {
	const VDXPixmapLayout& pxlsrc = *fa->src.mpPixmapLayout;
	VDXPixmapLayout& pxldst = *fa->dst.mpPixmapLayout;

	VDPixmapFormatEx format0 = ExtractBaseFormat(pxlsrc.format);
	format0.colorSpaceMode = ExtractColorSpace(fa->src.mpPixmap);
	format0.colorRangeMode = ExtractColorRange(fa->src.mpPixmap);

	VDPixmapFormatEx format = mFormat;
	if (format.format==0) format.format = format0.format;
	if (format.colorSpaceMode==0) format.colorSpaceMode = format0.colorSpaceMode;
	if (format.colorRangeMode==0) format.colorRangeMode = format0.colorRangeMode;
	format = VDPixmapFormatCombine(format, 0);

	if (pxlsrc.format == 255)
		return FILTERPARAM_NOT_SUPPORTED;

	if (pxlsrc.format != format)
		return FILTERPARAM_NOT_SUPPORTED;

	if (VDPixmapFormatMatrixType(format)==1) {
		if (fma && fma->fmpixmap) {
			FilterModPixmapInfo* info = fma->fmpixmap->GetPixmapInfo(fa->src.mpPixmap);
			info->colorSpaceMode = format.colorSpaceMode;
			info->colorRangeMode = format.colorRangeMode;
		}
	}

	pxldst.pitch = pxlsrc.pitch;

	return FILTERPARAM_SUPPORTS_ALTFORMATS | FILTERPARAM_PURE_TRANSFORM;
}

void VDVFilterConvertFormat::Run() {
}

void VDVFilterConvertFormatConfigDialog::redo() {
	filter->mFormat = mFormat;
	if (fp) fp->RedoSystem();
}

bool VDVFilterConvertFormat::Configure(VDXHWND hwnd) {
	VDVFilterConvertFormatConfigDialog dlg(mFormat);
	dlg.fp = fa->ifp2;
	dlg.filter = this;
	VDPixmapFormatEx oldFormat = mFormat;

	if (!dlg.ShowDialog((VDGUIHandle)hwnd)) {
		mFormat = oldFormat;
		return false;
	}

	mFormat = dlg.mFormat;
	return true;
}

void VDVFilterConvertFormat::GetSettingString(char *buf, int maxlen) {
	VDStringA s;
	if (mFormat.format==0) s += "*"; else s += VDPixmapGetInfo(mFormat).name;
	if (mFormat.format==0 || VDPixmapFormatMatrixType(mFormat)) {
		if (mFormat.colorSpaceMode==0) s += "-*";
		if (mFormat.colorSpaceMode==nsVDXPixmap::kColorSpaceMode_709) s += "-709";
		if (mFormat.colorRangeMode==0) s += "-*";
		if (mFormat.colorRangeMode==nsVDXPixmap::kColorRangeMode_Full) s += "-FR";
	}
	if (mFormat.fullEqual(0)) s = "No change";
	SafePrintf(buf, maxlen, " (%s)", s.c_str());
}

void VDVFilterConvertFormat::GetScriptString(char *buf, int maxlen) {
	int combo = VDPixmapFormatCombine(mFormat, 0);
	if (mFormat.fullEqual(VDPixmapFormatNormalize(combo)) || VDPixmapFormatMatrixType(mFormat)==0) {
		_snprintf(buf, maxlen, "Config(%d)", combo);
	} else {
		_snprintf(buf, maxlen, "Config(%d,%d,%d)", mFormat.format, mFormat.colorSpaceMode, mFormat.colorRangeMode);
	}
}

void VDVFilterConvertFormat::ScriptConfig(IVDXScriptInterpreter *, const VDXScriptValue *argv, int argc) {
	mFormat = VDPixmapFormatNormalize(argv[0].asInt());
}

void VDVFilterConvertFormat::ScriptConfig3(IVDXScriptInterpreter *, const VDXScriptValue *argv, int argc) {
	mFormat.format = VDPixmapFormatNormalize(argv[0].asInt());
	mFormat.colorSpaceMode = (nsVDXPixmap::ColorSpaceMode)argv[1].asInt();
	mFormat.colorRangeMode = (nsVDXPixmap::ColorRangeMode)argv[2].asInt();
}

VDXVF_BEGIN_SCRIPT_METHODS(VDVFilterConvertFormat)
VDXVF_DEFINE_SCRIPT_METHOD(VDVFilterConvertFormat, ScriptConfig, "i")
VDXVF_DEFINE_SCRIPT_METHOD(VDVFilterConvertFormat, ScriptConfig3, "iii")
VDXVF_END_SCRIPT_METHODS()

extern const VDXFilterDefinition2 g_VDVFConvertFormat = VDXVideoFilterDefinition<VDVFilterConvertFormat>(NULL, "convert format", "Converts video to a different color space or color encoding.");

// warning C4505: 'VDXVideoFilter::[thunk]: __thiscall VDXVideoFilter::`vcall'{24,{flat}}' }'' : unreferenced local function has been removed
#pragma warning(disable: 4505)
