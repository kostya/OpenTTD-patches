/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file newgrf_sl.cpp Code handling saving and loading of newgrf config */

#include "../stdafx.h"
#include "../fios.h"

#include "saveload_buffer.h"
#include "newgrf_sl.h"

/** Save and load the mapping between a spec and the NewGRF it came from. */
static const SaveLoad _newgrf_mapping_desc[] = {
	SLE_VAR(EntityIDMapping, grfid,         SLE_UINT32),
	SLE_VAR(EntityIDMapping, entity_id,     SLE_UINT8),
	SLE_VAR(EntityIDMapping, substitute_id, SLE_UINT8),
	SLE_END()
};

/**
 * Save a GRF ID + local id -> OpenTTD's id mapping.
 * @param mapping The mapping to save.
 */
void Save_NewGRFMapping(SaveDumper *dumper, const OverrideManagerBase &mapping)
{
	for (uint i = 0; i < mapping.GetMaxMapping(); i++) {
		dumper->WriteElement(i, &mapping.mapping_ID[i], _newgrf_mapping_desc);
	}
}

/**
 * Load a GRF ID + local id -> OpenTTD's id mapping.
 * @param mapping The mapping to load.
 */
void Load_NewGRFMapping(LoadBuffer *reader, OverrideManagerBase &mapping)
{
	/* Clear the current mapping stored.
	 * This will create the manager if ever it is not yet done */
	mapping.ResetMapping();

	uint max_id = mapping.GetMaxMapping();

	int index;
	while ((index = reader->IterateChunk()) != -1) {
		if ((uint)index >= max_id) throw SlCorrupt("Too many NewGRF entity mappings");
		reader->ReadObject(&mapping.mapping_ID[index], _newgrf_mapping_desc);
	}
}


static const SaveLoad _grfconfig_desc[] = {
	SLE_STR(GRFConfig, filename,     SLS_NONE),
	SLE_VAR(GRFConfig, ident.grfid,  SLE_UINT32),
	SLE_ARR(GRFConfig, ident.md5sum, SLE_UINT8,  16),
	SLE_VAR(GRFConfig, version,      SLE_UINT32,       0, , 151, ),
	SLE_ARR(GRFConfig, param,        SLE_UINT32, 0x80),
	SLE_VAR(GRFConfig, num_params,   SLE_UINT8),
	SLE_VAR(GRFConfig, palette,      SLE_UINT8,        0, , 101, ),
	SLE_END()
};


static void Save_NGRF(SaveDumper *dumper)
{
	int index = 0;

	for (GRFConfig *c = _grfconfig; c != NULL; c = c->next) {
		if (HasBit(c->flags, GCF_STATIC)) continue;
		dumper->WriteElement(index++, c, _grfconfig_desc);
	}
}


static void Load_NGRF_common(LoadBuffer *reader, GRFConfig *&grfconfig)
{
	ClearGRFConfigList(&grfconfig);
	while (reader->IterateChunk() != -1) {
		GRFConfig *c = new GRFConfig();
		reader->ReadObject(c, _grfconfig_desc);
		if (reader->IsOTTDVersionBefore(101)) c->SetSuitablePalette();
		AppendToGRFConfigList(&grfconfig, c);
	}
}

static void Load_NGRF(LoadBuffer *reader)
{
	Load_NGRF_common(reader, _grfconfig);

	if (_game_mode == GM_MENU) {
		/* Intro game must not have NewGRF. */
		if (_grfconfig != NULL) throw SlCorrupt("The intro game must not use NewGRF");

		/* Activate intro NewGRFs (townnames) */
		ResetGRFConfig(false);
	} else {
		/* Append static NewGRF configuration */
		AppendStaticGRFConfigs(&_grfconfig);
	}
}

static void Check_NGRF(LoadBuffer *reader)
{
	Load_NGRF_common(reader, _load_check_data.grfconfig);
}

extern const ChunkHandler _newgrf_chunk_handlers[] = {
	{ 'NGRF', Save_NGRF, Load_NGRF, NULL, Check_NGRF, CH_ARRAY | CH_LAST }
};
