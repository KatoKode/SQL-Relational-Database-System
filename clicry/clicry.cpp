//-----------------------------------------------------------------------------
// Desc: Private/Public Key Encryption/Decryption Library
//
// Copyright © 2019 J McIntosh
//
//   This file is part of redB.
//
//   redB is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; version 3 of the License.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with redB.  If not, see <https://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------

// https://www.gnupg.org/software/libgcrypt/index.html
// https://www.gnupg.org/documentation/manuals/gcrypt.pdf

#include "clicry.h"

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS CLICRY: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  bool clicry::encrypt (size_t inlen, uint8_t* inbuf, size_t* outlen,
      uint8_t* outbuf)
  {
    // The Libgcrypt Reference Manual, v1.8.2 23, Nov. 2017
  
    if (not is_initialized()) return false;
 
    gcry_error_t err = 0;
  
    // 12.3 MPI formats, Pg 74
    // The following functions are used to convert between an external repre-
    // sentation of an MPI and the internal one of Libgcrypt.
    gcry_mpi_t mpi;
    err = gcry_mpi_scan(&mpi, GCRYMPI_FMT_USG, inbuf, inlen, nullptr);
    //  Convert the external representation of an integer stored in 'buffer'
    //  with a length of 'buflen' into a newly created MPI returned which will
    //  be stored at the address of 'r_mpi'. For certain formats the length
    //  argument is not required and should be passed as 0. A 'buflen' larger
    //  than 16 MiByte will be rejected. After a successful operation the vari-
    //  able 'nscanned' receives the number of bytes actually scanned unless
    //  'nscanned' was given as NULL. 'format' describes the format of the MPI
    //  as stored in buffer:
    if (err) return false;

    // 11.2 Working with S-expressions, Pg 67-68
    // There are several functions to create an Libgcrypt S-expression object
    // from its external representation or from a string template. There is
    // also a function to convert the internal representation back into one of
    // the external formats:
    gcry_sexp_t sexp;
    err = gcry_sexp_build(&sexp, nullptr, "(data (flags raw) (value %m))", mpi);
    //  This function creates an internal S-expression from the string template
    //  format and stores it at the address of r sexp. If there is a parsing
    //  error, the function returns an appropriate error code and stores the
    //  offset into format where the parsing stopped in erroff. The function
    //  supports a couple of printf-like formatting characters and expects
    //  arguments for some of these escape sequences right after format.
    //  ‘%m’  The next argument is expected to be of type gcry_mpi_t and a copy
    //        of its value is inserted into the resulting S-expression. The MPI
    //        is stored as a signed integer.
    // 12.2 Basic functions, Pg 73
    gcry_mpi_release(mpi);
    //  Release the MPI 'a' and free all associated resources. Passing NULL
    //  is allowed and ignored. When a MPI stored in the "secure memory" is
    //  released, that memory gets wiped out immediately.
    if (err) return false;
  
    // 6.3 Cryptographic Functions, Pg 40
    gcry_sexp_t encry;
    err = gcry_pk_encrypt(&encry, sexp, m_pubkey);
    //  The 'data' to be encrypted can either be in the simple old format,
    //  which is a very simple S-expression consisting only of one MPI, or it
    //  may be a more complex S-expression which also allows to specify flags
    //  for operation, like e.g. padding rules. If you don’t want to let Lib-
    //  gcrypt handle the padding, you must pass an appropriate MPI using this
    //  expression for 'data': (data (flags raw) (value mpi ))
    //  This has the same semantics as the old style MPI only way. MPI is the
    //  actual data, already padded appropriate for your protocol. Most RSA
    //  based systems however use PKCS#1 padding and so you can use this S-
    //  expression for 'data': (data (flags pkcs1) (value block ))
    //  Here, the "flags" list has the "pkcs1" flag which let the function know
    //  that it should provide PKCS#1 block type 2 padding. The actual data to
    //  be encrypted is passed as a string of octets in block. The function
    //  checks that this data actually can be used with the given key, does the
    //  padding and encrypts it. If the function could successfully perform the
    //  encryption, the return value will be 0 and a new S-expression with the
    //  encrypted result is allocated and assigned to the variable at the
    //  address of 'r_ciph'. The caller is responsible to release this value
    //  using gcry_sexp_release. In case of an error, an error code is return-
    //  ed and 'r_ciph' will be set to NULL.
    //  The returned S-expression has this format when used with RSA:
    //  (enc-val (rsa (a a-mpi )))
    //  Where a-mpi is an MPI with the result of the RSA operation. When using
    //  the Elgamal algorithm, the return value will have this format:
    //  (enc-val (elg (a a-mpi ) (b b-mpi )))
    //  Where a-mpi and b-mpi are MPIs with the result of the Elgamal encryp-
    //  tion operation.
    // 11.2 Working with S-expressions, Pg 68
    gcry_sexp_release(sexp);
    //  Release the S-expression object 'sexp'. If the S-expression is stored in
    //  secure memory it explicitly zeroises that memory; note that this is done
    //  in addition to the zeroisation always done when freeing secure memory.
    // 12.2 Basic functions, Pg 73
    if (err) return false;

    // 11.2 Working with S-expressions, Pg 68-69
    // ... used to convert the internal representation back into a regular
    // external S-expression format and to show the structure for debugging.
    *outlen = gcry_sexp_sprint(encry, GCRYSEXP_FMT_DEFAULT, outbuf, *outlen);
    //  Copies the S-expression object 'sexp' into 'buffer' using the format
    //  specified in 'mode'. 'maxlength' must be set to the allocated length
    //  of 'buffer'. The function returns the actual length of valid bytes
    //  put into 'buffer' or 0 if the provided 'buffer' is too short.  Passing
    //  NULL for 'buffer' returns the required length for 'buffer'. For conven-
    //  ience reasons an extra byte with value 0 is appended to the 'buffer'.
    //  The following formats are supported:
    //    GCRYSEXP_FMT_DEFAULT
    //    Returns a convenient external S-expression representation.

    // 11.2 Working with S-expressions, Pg 68
    gcry_sexp_release(encry);
    //  Release the S-expression object 'sexp'. If the S-expression is stored in
    //  secure memory it explicitly zeroises that memory; note that this is done
    //  in addition to the zeroisation always done when freeing secure memory.

    return true;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS CLICRY: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void clicry::init()
  {
    // The Libgcrypt Reference Manual, v1.8.2 23, Nov. 2017

    // 2.4 Initializing the library, Pg 4
    // If you have to protect your keys or other information in memory against
    // being swapped out to disk and to enable an automatic overwrite of used
    // and freed memory, you need to initialize Libgcrypt this way:
    if (gcry_check_version (GCRYPT_VERSION) == nullptr) {
      m_is_initialized = false;
      return;
    }

    gcry_error_t err = 0;
  
    // We don't want to see any warnings, e.g. because we have not yet parsed
    // program options which might be used to suppress such warnings.
    err = gcry_control (GCRYCTL_SUSPEND_SECMEM_WARN);
  
    // ... If required, other initialization goes here.  Note that the process
    // might still be running with increased privileges and that the secure
    // memory has not been intialized.

    // Allocate a pool of 16k secure memory. This makes the secure memory
    // available and also drops privileges where needed. Note that by using
    // functions like gcry_xmalloc_secure and gcry_mpi_snew Libgcrypt may
    // expand the secure memory pool with memory which lacks the property of
    // not being swapped out to disk.
    err |= gcry_control (GCRYCTL_INIT_SECMEM, 16384, 0);
  
    // It is now okay to let Libgcrypt complain when there was/is a problem
    // with the secure memory.
    err |= gcry_control (GCRYCTL_RESUME_SECMEM_WARN);

    // ... If required, other initialization goes here.

    // Tell Libgcrypt that initialization has completed.
    err |= gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);

    if (err) { m_is_initialized = false; return; }
  }

  /****************************************************************************
   *
   */
  void clicry::buffer_to_pubkey (size_t buflen, uint8_t* buffer) {
    // The Libgcrypt Reference Manual, v1.8.2 23, Nov. 2017
  
    if (not is_initialized()) return;

    // There are several functions to create an Libgcrypt S-expression object
    // from its external representation or from a string template. There is
    // also a function to convert the internal representation back into one of
    // the external formats:
    (void) gcry_sexp_new(&m_pubkey, buffer, buflen, 0);
    //  This is the generic function to create an new S-expression object from
    //  its external representation in 'buffer' of length 'bytes'. On success
    //  the result is stored at the address given by 'r_sexp'. With autodetect
    //  set to 0, the data in 'buffer' is expected to be in canonized format,
    //  with autodetect set to 1 the parses any of the defined external for-
    //  mats. If 'buffer' does not hold a valid S-expression an error code is
    //  returned and 'r_sexp' set to NULL. Note that the caller is responsible
    //  for releasing the newly allocated S-expression using gcry_sexp_release.
  }

} // namespace

