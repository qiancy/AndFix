/*
 *
 * Copyright (c) 2011 The Android Open Source Project
 * Copyright (c) 2015, alipay.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * 	art_5_0.h
 *
 * @author : sanping.li@alipay.com
 *
 */
#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <fcntl.h>
#include <dlfcn.h>

#include <stdint.h>    /* C99 */

namespace art {
namespace mirror {
class Object {
public:
	void* klass_;

	uint32_t monitor_;
};
class Class: public Object {
public:
	// Interface method table size. Increasing this value reduces the chance of two interface methods
	// colliding in the interface method table but increases the size of classes that implement
	// (non-marker) interfaces.
	static constexpr size_t kImtSize = 64;	//IMT_SIZE;
	// defining class loader, or NULL for the "bootstrap" system loader
	void* class_loader_;
	// For array classes, the component class object for instanceof/checkcast
	// (for String[][][], this will be String[][]). NULL for non-array classes.
	void* component_type_;
	// DexCache of resolved constant pool entries (will be NULL for classes generated by the
	// runtime such as arrays and primitive classes).
	void* dex_cache_;
	// static, private, and <init> methods
	void* direct_methods_;
	// instance fields
	//
	// These describe the layout of the contents of an Object.
	// Note that only the fields directly declared by this class are
	// listed in ifields; fields declared by a superclass are listed in
	// the superclass's Class.ifields.
	//
	// All instance fields that refer to objects are guaranteed to be at
	// the beginning of the field list.  num_reference_instance_fields_
	// specifies the number of reference fields.
	void* ifields_;
	// The interface table (iftable_) contains pairs of a interface class and an array of the
	// interface methods. There is one pair per interface supported by this class.  That means one
	// pair for each interface we support directly, indirectly via superclass, or indirectly via a
	// superinterface.  This will be null if neither we nor our superclass implement any interfaces.
	//
	// Why we need this: given "class Foo implements Face", declare "Face faceObj = new Foo()".
	// Invoke faceObj.blah(), where "blah" is part of the Face interface.  We can't easily use a
	// single vtable.
	//
	// For every interface a concrete class implements, we create an array of the concrete vtable_
	// methods for the methods in the interface.
	void* iftable_;
	// Interface method table (imt), for quick "invoke-interface".
	void* imtable_;
	// Descriptor for the class such as "java.lang.Class" or "[C". Lazily initialized by ComputeName
	void* name_;
	// Static fields
	void* sfields_;
	// The superclass, or NULL if this is java.lang.Object, an interface or primitive type.
	void* super_class_;
	// If class verify fails, we must return same error on subsequent tries.
	void* verify_error_class_;
	// Virtual methods defined in this class; invoked through vtable.
	void* virtual_methods_;
	// Virtual method table (vtable), for use by "invoke-virtual".  The vtable from the superclass is
	// copied in, and virtual methods from our class either replace those from the super or are
	// appended. For abstract classes, methods may be created in the vtable that aren't in
	// virtual_ methods_ for miranda methods.
	void* vtable_;
	// Access flags; low 16 bits are defined by VM spec.
	uint32_t access_flags_;
	// Total size of the Class instance; used when allocating storage on gc heap.
	// See also object_size_.
	uint32_t class_size_;
	// Tid used to check for recursive <clinit> invocation.
	pid_t clinit_thread_id_;
	// ClassDef index in dex file, -1 if no class definition such as an array.
	// TODO: really 16bits
	int32_t dex_class_def_idx_;
	// Type index in dex file.
	// TODO: really 16bits
	int32_t dex_type_idx_;
	// Number of instance fields that are object refs.
	uint32_t num_reference_instance_fields_;
	// Number of static fields that are object refs,
	uint32_t num_reference_static_fields_;
	// Total object size; used when allocating storage on gc heap.
	// (For interfaces and abstract classes this will be zero.)
	// See also class_size_.
	uint32_t object_size_;
	// Primitive type value, or Primitive::kPrimNot (0); set for generated primitive classes.
	void* primitive_type_;
	// Bitmap of offsets of ifields.
	uint32_t reference_instance_offsets_;
	// Bitmap of offsets of sfields.
	uint32_t reference_static_offsets_;
	// State of class initialization.
	void* status_;
	// TODO: ?
	// initiating class loader list
	// NOTE: for classes with low serialNumber, these are unused, and the
	// values are kept in a table in gDvm.
	// InitiatingLoaderList initiating_loader_list_;
	// The following data exist in real class objects.
	// Embedded Imtable, for class object that's not an interface, fixed size.
	void* embedded_imtable_[0];
	// Embedded Vtable, for class object that's not an interface, variable size.
	void* embedded_vtable_[0];
	// Static fields, variable size.
	uint32_t fields_[0];
	// java.lang.Class
	static void* java_lang_Class_;
};

class ArtField: public Object  {
public:
	void* declaring_class_;
	int32_t access_flags_;
	int32_t field_dex_idx_;
	int32_t offset_;
};

class ArtMethod: public Object {
public:
	// Field order required by test "ValidateFieldOrderOfJavaCppUnionClasses".
	// The class we are a part of
	Class* declaring_class_;

	// short cuts to declaring_class_->dex_cache_ member for fast compiled code access
	void* dex_cache_initialized_static_storage_;

	// short cuts to declaring_class_->dex_cache_ member for fast compiled code access
	void* dex_cache_resolved_methods_;

	// short cuts to declaring_class_->dex_cache_ member for fast compiled code access
	void* dex_cache_resolved_types_;

	// short cuts to declaring_class_->dex_cache_ member for fast compiled code access
	void* dex_cache_strings_;

	// Access flags; low 16 bits are defined by spec.
	uint32_t access_flags_;

	// Offset to the CodeItem.
	uint32_t code_item_offset_;

	// Architecture-dependent register spill mask
	uint32_t core_spill_mask_;

	// Compiled code associated with this method for callers from managed code.
	// May be compiled managed code or a bridge for invoking a native method.
	// TODO: Break apart this into portable and quick.
	const void* entry_point_from_compiled_code_;

	// Called by the interpreter to execute this method.
	void* entry_point_from_interpreter_;

	// Architecture-dependent register spill mask
	uint32_t fp_spill_mask_;

	// Total size in bytes of the frame
	size_t frame_size_in_bytes_;

	// Garbage collection map of native PC offsets (quick) or dex PCs (portable) to reference bitmaps.
	const uint8_t* gc_map_;

	// Mapping from native pc to dex pc
	const uint32_t* mapping_table_;

	// Index into method_ids of the dex file associated with this method
	uint32_t method_dex_index_;

	// For concrete virtual methods, this is the offset of the method in Class::vtable_.
	//
	// For abstract methods in an interface class, this is the offset of the method in
	// "iftable_->Get(n)->GetMethodArray()".
	//
	// For static and direct methods this is the index in the direct methods table.
	uint32_t method_index_;

	// The target native method registered with this method
	const void* native_method_;

	// When a register is promoted into a register, the spill mask holds which registers hold dex
	// registers. The first promoted register's corresponding dex register is vmap_table_[1], the Nth
	// is vmap_table_[N]. vmap_table_[0] holds the length of the table.
	const uint16_t* vmap_table_;

	static void* java_lang_reflect_ArtMethod_;
};

}

}
