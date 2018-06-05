// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_ZUCCHINI_DISASSEMBLER_DEX_H_
#define COMPONENTS_ZUCCHINI_DISASSEMBLER_DEX_H_

#include <stdint.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "base/macros.h"
#include "components/zucchini/disassembler.h"
#include "components/zucchini/image_utils.h"
#include "components/zucchini/type_dex.h"

namespace zucchini {

// For consistency, let "canonical order" of DEX data types be the order defined
// in https://source.android.com/devices/tech/dalvik/dex-format "Type Codes"
// section.

class DisassemblerDex : public Disassembler {
 public:
  // Pools follow canonical order.
  enum ReferencePool : uint8_t {
    kStringId,
    kTypeId,
    kProtoId,
    kFieldId,
    kMethodId,
    // kClassDef,  // Unused
    // kCallSiteId, // Unused
    // kMethodHandle, // Unused
    kTypeList,
    kAnnotationSetRefList,
    kAnnotionSet,
    kClassData,
    kCode,
    kStringData,
    kAnnotation,
    kEncodedArray,
    kAnnotationsDirectory,
    // kCallSite, // Unused
    kNumPools
  };

  // Types are grouped and ordered by target ReferencePool. This is required by
  // Zucchini-apply, which visits references by type order and sequentially
  // handles pools in the same order. Type-pool association is established in
  // MakeReferenceGroups(), and verified by a unit test.
  enum ReferenceType : uint8_t {
    kTypeIdToDescriptorStringId,  // kStringId
    kProtoIdToShortyStringId,
    kFieldIdToNameStringId,
    kMethodIdToNameStringId,
    kClassDefToSourceFileStringId,
    kCodeToStringId16,
    kCodeToStringId32,

    kProtoIdToReturnTypeId,  // kTypeId
    kFieldIdToClassTypeId,
    kFieldIdToTypeId,
    kMethodIdToClassTypeId,
    kClassDefToClassTypeId,
    kClassDefToSuperClassTypeId,
    // kTypeListToTypeId,  // Unused
    kCodeToTypeId,

    kMethodIdToProtoId,  // kProtoId

    kCodeToFieldId,  // kFieldId
    // kAnnotationsDirectoryToFieldId,

    kCodeToMethodId,  // kMethodId
    // kAnnotationsDirectoryToMethodId,
    // kAnnotationsDirectoryToParameterMethodId,

    kProtoIdToParametersTypeList,  // kTypeList
    kClassDefToInterfacesTypeList,

    // kAnnotationsDirectoryToParameterAnnotationSetRef,  // kAnnotationSetRef,

    // kAnnotationSetRefListToAnnotationSet,  // kAnnotationSet,
    // kAnnotationsDirectoryToClassAnnotationSet,
    // kAnnotationsDirectoryToFieldAnnotationSet,
    // kAnnotationsDirectoryToMethodAnnotationSet,

    kClassDefToClassData,  // kClassData

    kCodeToRelCode8,  // kCode
    kCodeToRelCode16,
    kCodeToRelCode32,

    kStringIdToStringData,  // kStringData

    // kAnnotationSetToAnnotation,  // kAnnotation

    kClassDefToStaticValuesEncodedArray,  // kEncodedArrayItem

    kClassDefToAnnotationDirectory,  // kAnnotationsDirectory

    // Intentionally ignored references (never appeared in test corpus).
    // kMethodHandleToFieldId,
    // kMethodHandleToMethodId,
    // kCallSiteIdToCallSite,

    kNumTypes
  };

  DisassemblerDex();
  ~DisassemblerDex() override;

  // Applies quick checks to determine if |image| *may* point to the start of an
  // executable. Returns true on success.
  static bool QuickDetect(ConstBufferView image);

  // Disassembler:
  ExecutableType GetExeType() const override;
  std::string GetExeTypeString() const override;
  std::vector<ReferenceGroup> MakeReferenceGroups() const override;

  // Functions that return reference readers. These follow canonical order of
  // *locations* (unlike targets for ReferenceType). This allows functions with
  // similar parsing logic to appear togeter.
  std::unique_ptr<ReferenceReader> MakeReadStringIdToStringData(offset_t lo,
                                                                offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadTypeIdToDescriptorStringId32(
      offset_t lo,
      offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadProtoIdToShortyStringId32(
      offset_t lo,
      offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadProtoIdToReturnTypeId32(offset_t lo,
                                                                   offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadProtoIdToParametersTypeList(
      offset_t lo,
      offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadFieldToClassTypeId16(offset_t lo,
                                                                offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadFieldToTypeId16(offset_t lo,
                                                           offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadFieldToNameStringId32(offset_t lo,
                                                                 offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadMethodIdToClassTypeId16(offset_t lo,
                                                                   offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadMethodIdToProtoId16(offset_t lo,
                                                               offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadMethodIdToNameStringId32(
      offset_t lo,
      offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadClassDefToClassTypeId32(offset_t lo,
                                                                   offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadClassDefToSuperClassTypeId32(
      offset_t lo,
      offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadClassDefToInterfacesTypeList(
      offset_t lo,
      offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadClassDefToSourceFileStringId32(
      offset_t lo,
      offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadClassDefToAnnotationDirectory(
      offset_t lo,
      offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadClassDefToClassData(offset_t lo,
                                                               offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadClassDefToStaticValuesEncodedArray(
      offset_t lo,
      offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadCodeToStringId16(offset_t lo,
                                                            offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadCodeToStringId32(offset_t lo,
                                                            offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadCodeToTypeId16(offset_t lo,
                                                          offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadCodeToFieldId16(offset_t lo,
                                                           offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadCodeToMethodId16(offset_t lo,
                                                            offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadCodeToRelCode8(offset_t lo,
                                                          offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadCodeToRelCode16(offset_t lo,
                                                           offset_t hi);
  std::unique_ptr<ReferenceReader> MakeReadCodeToRelCode32(offset_t lo,
                                                           offset_t hi);

  // Functions that return reference writers. Different readers may share a
  // common writer. Therefore these loosely follow canonical order of locations,
  std::unique_ptr<ReferenceWriter> MakeWriteStringId16(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteStringId32(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteTypeId16(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteTypeId32(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteProtoId16(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteFieldId16(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteFieldId32(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteMethodId16(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteMethodId32(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteRelCode8(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteRelCode16(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteRelCode32(MutableBufferView image);
  std::unique_ptr<ReferenceWriter> MakeWriteAbs32(MutableBufferView image);

 private:
  friend Disassembler;
  using MapItemMap = std::map<uint16_t, const dex::MapItem*>;

  // Disassembler:
  bool Parse(ConstBufferView image) override;

  bool ParseHeader();

  const dex::HeaderItem* header_ = nullptr;
  int dex_version_ = 0;
  MapItemMap map_item_map_ = {};
  dex::MapItem string_map_item_ = {};
  dex::MapItem type_map_item_ = {};
  dex::MapItem proto_map_item_ = {};
  dex::MapItem field_map_item_ = {};
  dex::MapItem method_map_item_ = {};
  dex::MapItem class_def_map_item_ = {};
  dex::MapItem code_map_item_ = {};

  // Sorted list of offsets of code items in |image_|.
  std::vector<offset_t> code_item_offsets_;

  DISALLOW_COPY_AND_ASSIGN(DisassemblerDex);
};

}  // namespace zucchini

#endif  // COMPONENTS_ZUCCHINI_DISASSEMBLER_DEX_H_
