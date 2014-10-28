/* Copyright 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ecma-alloc.h"
#include "ecma-boolean-object.h"
#include "ecma-builtins.h"
#include "ecma-conversion.h"
#include "ecma-exceptions.h"
#include "ecma-gc.h"
#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "ecma-objects.h"
#include "ecma-try-catch-macro.h"
#include "globals.h"

#define ECMA_BUILTINS_INTERNAL
#include "ecma-builtins-internal.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmabuiltins
 * @{
 *
 * \addtogroup boolean ECMA Boolean object built-in
 * @{
 */

/**
 * List of the Boolean object built-in object value properties in format 'macro (name, value)'.
 */
#define ECMA_BUILTIN_BOOLEAN_OBJECT_OBJECT_VALUES_PROPERTY_LIST(macro) \
  macro (ECMA_MAGIC_STRING_PROTOTYPE, ecma_builtin_get (ECMA_BUILTIN_ID_BOOLEAN_PROTOTYPE))

#define ECMA_BUILTIN_BOOLEAN_OBJECT_NUMBER_VALUES_PROPERTY_LIST(macro) \
  macro (ECMA_MAGIC_STRING_LENGTH, 1)

/**
 * List of the Boolean object's built-in property names
 */
static const ecma_magic_string_id_t ecma_builtin_boolean_property_names[] =
{
#define VALUE_PROP_LIST(name, value) name,
#define ROUTINE_PROP_LIST(name, c_function_name, args_boolean, length) name,
  ECMA_BUILTIN_BOOLEAN_OBJECT_OBJECT_VALUES_PROPERTY_LIST (VALUE_PROP_LIST)
  ECMA_BUILTIN_BOOLEAN_OBJECT_NUMBER_VALUES_PROPERTY_LIST (VALUE_PROP_LIST)
#undef VALUE_PROP_LIST
#undef ROUTINE_PROP_LIST
};

/**
 * Number of the Boolean object's built-in properties
 */
static const ecma_length_t ecma_builtin_boolean_property_number = (sizeof (ecma_builtin_boolean_property_names) /
                                                                   sizeof (ecma_magic_string_id_t));

/**
 * If the property's name is one of built-in properties of the Boolean object
 * that is not instantiated yet, instantiate the property and
 * return pointer to the instantiated property.
 *
 * @return pointer property, if one was instantiated,
 *         NULL - otherwise.
 */
ecma_property_t*
ecma_builtin_boolean_try_to_instantiate_property (ecma_object_t *obj_p, /**< object */
                                                 ecma_string_t *prop_name_p) /**< property's name */
{
  JERRY_ASSERT (ecma_builtin_is (obj_p, ECMA_BUILTIN_ID_BOOLEAN));
  JERRY_ASSERT (ecma_find_named_property (obj_p, prop_name_p) == NULL);

  ecma_magic_string_id_t id;

  if (!ecma_is_string_magic (prop_name_p, &id))
  {
    return NULL;
  }

  int32_t index = ecma_builtin_bin_search_for_magic_string_id_in_array (ecma_builtin_boolean_property_names,
                                                                        ecma_builtin_boolean_property_number,
                                                                        id);

  if (index == -1)
  {
    return NULL;
  }

  JERRY_ASSERT (index >= 0 && (uint32_t) index < sizeof (uint64_t) * JERRY_BITSINBYTE);

  uint32_t bit;
  ecma_internal_property_id_t mask_prop_id;

  if (index >= 32)
  {
    mask_prop_id = ECMA_INTERNAL_PROPERTY_NON_INSTANTIATED_BUILT_IN_MASK_32_63;
    bit = (uint32_t) 1u << (index - 32);
  }
  else
  {
    mask_prop_id = ECMA_INTERNAL_PROPERTY_NON_INSTANTIATED_BUILT_IN_MASK_0_31;
    bit = (uint32_t) 1u << index;
  }

  ecma_property_t *mask_prop_p = ecma_find_internal_property (obj_p, mask_prop_id);
  if (mask_prop_p == NULL)
  {
    mask_prop_p = ecma_create_internal_property (obj_p, mask_prop_id);
    mask_prop_p->u.internal_property.value = 0;
  }

  uint32_t bit_mask = mask_prop_p->u.internal_property.value;

  if (bit_mask & bit)
  {
    return NULL;
  }

  bit_mask |= bit;

  mask_prop_p->u.internal_property.value = bit_mask;

  ecma_value_t value = ecma_make_simple_value (ECMA_SIMPLE_VALUE_EMPTY);
  ecma_property_writable_value_t writable = ECMA_PROPERTY_WRITABLE;
  ecma_property_enumerable_value_t enumerable = ECMA_PROPERTY_NOT_ENUMERABLE;
  ecma_property_configurable_value_t configurable = ECMA_PROPERTY_CONFIGURABLE;

  switch (id)
  {
#define CASE_VALUE_PROP_LIST(name, value) case name:
    ECMA_BUILTIN_BOOLEAN_OBJECT_OBJECT_VALUES_PROPERTY_LIST (CASE_VALUE_PROP_LIST)
    ECMA_BUILTIN_BOOLEAN_OBJECT_NUMBER_VALUES_PROPERTY_LIST (CASE_VALUE_PROP_LIST)
#undef CASE_VALUE_PROP_LIST
    {
      writable = ECMA_PROPERTY_NOT_WRITABLE;
      enumerable = ECMA_PROPERTY_NOT_ENUMERABLE;
      configurable = ECMA_PROPERTY_NOT_CONFIGURABLE;

      switch (id)
      {
#define CASE_OBJECT_VALUE_PROP_LIST(name, value_obj) case name: { value = ecma_make_object_value (value_obj); break; }
        ECMA_BUILTIN_BOOLEAN_OBJECT_OBJECT_VALUES_PROPERTY_LIST (CASE_OBJECT_VALUE_PROP_LIST)
#undef CASE_OBJECT_VALUE_PROP_LIST
#define CASE_NUMBER_VALUE_PROP_LIST(name, value_num) case name: { ecma_number_t *num_p = ecma_alloc_number (); \
                                                                  *num_p = (ecma_number_t) value_num; \
                                                                  value = ecma_make_number_value (num_p); break; }
        ECMA_BUILTIN_BOOLEAN_OBJECT_NUMBER_VALUES_PROPERTY_LIST (CASE_NUMBER_VALUE_PROP_LIST)
#undef CASE_NUMBER_VALUE_PROP_LIST
        default:
        {
          JERRY_UNREACHABLE ();
        }
      }

      break;
    }

    default:
    {
      JERRY_UNREACHABLE ();
    }
  }

  ecma_property_t *prop_p = ecma_create_named_data_property (obj_p,
                                                             prop_name_p,
                                                             writable,
                                                             enumerable,
                                                             configurable);

  prop_p->u.named_data_property.value = ecma_copy_value (value, false);
  ecma_gc_update_may_ref_younger_object_flag_by_value (obj_p,
                                                       prop_p->u.named_data_property.value);

  ecma_free_value (value, true);

  return prop_p;
} /* ecma_builtin_boolean_try_to_instantiate_property */

/**
 * Dispatcher of the Boolean object's built-in routines
 *
 * @return completion-value
 *         Returned value must be freed with ecma_free_completion_value.
 */
ecma_completion_value_t
ecma_builtin_boolean_dispatch_routine (ecma_magic_string_id_t builtin_routine_id __unused, /**< Boolean object's
                                                                                               built-in routine's
                                                                                               name */
                                      ecma_value_t this_arg_value __unused, /**< 'this' argument value */
                                      ecma_value_t arguments_list [] __unused, /**< list of arguments
                                                                                    passed to routine */
                                      ecma_length_t arguments_boolean __unused) /**< length of arguments' list */
{
  JERRY_UNREACHABLE ();
} /* ecma_builtin_boolean_dispatch_routine */

/**
 * Get number of routine's parameters
 *
 * @return number of parameters
 */
ecma_length_t
ecma_builtin_boolean_get_routine_parameters_number (ecma_magic_string_id_t builtin_routine_id __unused) /**< built-in
                                                                                                             routine's
                                                                                                             name */
{
  JERRY_UNREACHABLE ();
} /* ecma_builtin_boolean_get_routine_parameters_boolean */

/**
 * Handle calling [[Call]] of built-in Boolean object
 *
 * @return completion-value
 */
ecma_completion_value_t
ecma_builtin_boolean_dispatch_call (ecma_value_t *arguments_list_p, /**< arguments list */
                                    ecma_length_t arguments_list_len) /**< number of arguments */
{
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  ecma_value_t arg_value;

  if (arguments_list_len == 0)
  {
    arg_value = ecma_make_simple_value (ECMA_SIMPLE_VALUE_UNDEFINED);
  }
  else
  {
    arg_value = arguments_list_p [0];
  }

  return ecma_op_to_boolean (arg_value);
} /* ecma_builtin_boolean_dispatch_call */

/**
 * Handle calling [[Construct]] of built-in Boolean object
 *
 * @return completion-value
 */
ecma_completion_value_t
ecma_builtin_boolean_dispatch_construct (ecma_value_t *arguments_list_p, /**< arguments list */
                                         ecma_length_t arguments_list_len) /**< number of arguments */
{
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  if (arguments_list_len == 0)
  {
    return ecma_op_create_boolean_object (ecma_make_simple_value (ECMA_SIMPLE_VALUE_FALSE));
  }
  else
  {
    return ecma_op_create_boolean_object (arguments_list_p[0]);
  }
} /* ecma_builtin_boolean_dispatch_construct */

/**
 * @}
 * @}
 * @}
 */