/* Copyright JS Foundation and other contributors, http://js.foundation
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

/*
 * %TypedArray% description
 */
#ifndef NUMBER_VALUE
# define NUMBER_VALUE(name, number_value, prop_attributes)
#endif /* !NUMBER_VALUE */

#ifndef STRING_VALUE
# define STRING_VALUE(name, magic_string_id, prop_attributes)
#endif /* !STRING_VALUE */

#ifndef OBJECT_VALUE
# define OBJECT_VALUE(name, obj_builtin_id, prop_attributes)
#endif /* !OBJECT_VALUE */

#ifndef ROUTINE
# define ROUTINE(name, c_function_name, args_number, length_prop_value)
#endif /* !ROUTINE */

/* ES2015 22.2.2 */
NUMBER_VALUE (LIT_MAGIC_STRING_LENGTH,
              3,
              ECMA_PROPERTY_FIXED)

/* ES2015 22.2.2 */
STRING_VALUE (LIT_MAGIC_STRING_NAME,
              LIT_MAGIC_STRING_TYPED_ARRAY_UL,
              ECMA_PROPERTY_FIXED)

/* ES2015 22.2.2.3 */
OBJECT_VALUE (LIT_MAGIC_STRING_PROTOTYPE,
              ECMA_BUILTIN_ID_TYPEDARRAY_PROTOTYPE,
              ECMA_PROPERTY_FIXED)

/* Routine properties:
 *  (property name, C routine name, arguments number or NON_FIXED, value of the routine's length property) */

/* ES2015 22.2.2.1 */
ROUTINE (LIT_MAGIC_STRING_FROM, ecma_builtin_typedarray_from, NON_FIXED, 1)

/* ES2015 22.2.2.2 */
ROUTINE (LIT_MAGIC_STRING_OF, ecma_builtin_typedarray_of, NON_FIXED, 0)

#undef SIMPLE_VALUE
#undef NUMBER_VALUE
#undef STRING_VALUE
#undef OBJECT_VALUE
#undef ROUTINE
#undef ACCESSOR_READ_WRITE
#undef ACCESSOR_READ_ONLY