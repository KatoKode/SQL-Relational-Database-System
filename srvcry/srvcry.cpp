//-----------------------------------------------------------------------------
// Desc: Server Public Key Encryption/Decryption Library
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

#include "srvcry.h"

namespace red {

  int32_t const MIN_ZERO = 0;
  int32_t const MIN_RAND = std::numeric_limits<int32_t>::min();
  int32_t const MAX_RAND = std::numeric_limits<int32_t>::max();
  int32_t const RANGE_BASE {31};
  int32_t const MAX_RANGE {95};
  size_t const MAXHASH {63};
  size_t const BUFLEN {4096};

  int32_t const GCRY_CALLOC         = 5400;
  int32_t const GCRY_CIPHER_DECRYPT = 5401;
  int32_t const GCRY_CIPHER_ENCRYPT = 5402;
	int32_t const GCRY_CIPHER_GET_ALGO_BLKLEN = 5403;
  int32_t const GCRY_CIPHER_OPEN    = 5404;
  int32_t const GCRY_CIPHER_SETIV   = 5405;
  int32_t const GCRY_CIPHER_SETKEY  = 5406;
  int32_t const GCRY_HASH_FILE      = 5407;
  int32_t const GCRY_KEYP_FILE      = 5408;
  int32_t const GCRY_LIB_INIT       = 5409;
  int32_t const GCRY_PK_DECRYPT     = 5410;
 	int32_t const GCRY_PK_GENKEY      = 5411;
  int32_t const GCRY_PKEY_FILE      = 5412;
  int32_t const GCRY_SEXP_BUILD     = 5413;
  int32_t const GCRY_SEXP_NEW       = 5414;
  int32_t const GCRY_SEXP_NTH_MPI   = 5415;

  /****************************************************************************
   *
   */
  std::string generate_password () {
    int32_t PASSWD_SZ {0};
    while (PASSWD_SZ < 16) PASSWD_SZ = rand_int32(MIN_ZERO, MAX_RAND) % 64;
    std::ostringstream password;
    while (password.str().size() < static_cast<size_t>(PASSWD_SZ)) {
      int32_t r = rand_int32(MIN_RAND, MAX_RAND);
      password << std::hex << r;
    }
    return password.str();
  }

  /****************************************************************************
   *
   */
  void report (std::shared_ptr<logger> lgr,
      int32_t ERRNO, int32_t const code)
  {
    std::string msg = eno::get_msg(ERRNO);
    std::ostringstream oss;
    oss << msg << " <" << code << ">\n";
    lgr->write(red::CRITICAL, oss.str());
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS GCRY: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  uint8_t* srvcry::decrypt (size_t buflen, uint8_t* buffer)
  {
    // The Libgcrypt Reference Manual, v1.8.2 23, Nov. 2017
  
    gcry_error_t err;
  
    // 11.2 Working with S-expressions, Pg 67
    gcry_sexp_t encry;
    err = gcry_sexp_new(&encry, buffer, buflen, 0);
    //  This is the generic function to create an new S-expression object from
    //  its external representation in 'buffer' of length 'bytes'. On success
    //  the result is stored at the address given by 'r_sexp'. With autodetect
    //  set to 0, the data in 'buffer' is expected to be in canonized format,
    //  with autodetect set to 1 the parses any of the defined external formats.
    //  If buffer does not hold a valid S-expression an error code is returned
    //  and 'r_sexp' set to NULL. Note that the caller is responsible for re-
    //  leasing the newly allocated S-expression using gcry_sexp_release.
    if (err) {
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_SEXP_NEW);
      return nullptr;
    }
  
    // 6.3 Cryptographic Functions, Pg 40
    gcry_sexp_t decry;
    err = gcry_pk_decrypt(&decry, encry, m_prikey);
    //  The function returns 0 on success or an error code. The variable at the
    //  address of 'r_plain' will be set to NULL on error or receive the decrypt-
    //  ed value on success. The format of 'r_plain' is a simple S-expression part
    //  (i.e. not a valid one) with just one MPI if there was no flags element in
    //  'data'; if at least an empty flags is passed in 'data', the format is:
    //  (value plaintext )
    if (err) {
      // 11.2 Working with S-expressions, Pg 68
      gcry_sexp_release(encry);
      //  Release the S-expression object 'sexp'. If the S-expression is stored in
      //  secure memory it explicitly zeroises that memory; note that this is done
      //  in addition to the zeroisation always done when freeing secure memory.
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_PK_DECRYPT);
      return nullptr;
    }
  
    // 11.2 Working with S-expressions, Pg 68
    gcry_sexp_release(encry);
    //  Release the S-expression object 'sexp'. If the S-expression is stored in
    //  secure memory it explicitly zeroises that memory; note that this is done
    //  in addition to the zeroisation always done when freeing secure memory.
  
    // 11.2 Working with S-expressions, Pg 70
    gcry_mpi_t mpi = gcry_sexp_nth_mpi(decry, 0, GCRYMPI_FMT_USG);
    //  This function is used to get and convert data from a 'list'. This data is
    //  assumed to be an MPI stored in the format described by 'mpifmt' and re-
    //  turned as a standard Libgcrypt MPI. The caller must release this returned
    //  value using gcry_mpi_release. If there is no data at the given index, the
    //  index represents a list or the value can’t be converted to an MPI, NULL
    //  is returned. If you use this function to parse results of a public key
    //  function, you most likely want to use GCRYMPI_FMT_USG.
    if (mpi == nullptr) {
      // 11.2 Working with S-expressions, Pg 68
      gcry_sexp_release(decry);
      //  Release the S-expression object 'sexp'. If the S-expression is stored in
      //  secure memory it explicitly zeroises that memory; note that this is done
      //  in addition to the zeroisation always done when freeing secure memory.
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_SEXP_NTH_MPI);
      return nullptr;
    }
  
    // 11.2 Working with S-expressions, Pg 68
    gcry_sexp_release(decry);
    //  Release the S-expression object 'sexp'. If the S-expression is stored in
    //  secure memory it explicitly zeroises that memory; note that this is done
    //  in addition to the zeroisation always done when freeing secure memory.

    size_t const MPIBUFLEN = 1460;
    uint8_t* mpibuf = reinterpret_cast<uint8_t*>(calloc(1, MPIBUFLEN));
    if (mpibuf == nullptr) {
      // 12.2 Basic functions, Pg 73
      gcry_mpi_release(mpi);
      //  Release the MPI 'a' and free all associated resources. Passing NULL is
      //  allowed and ignored. When a MPI stored in the "secure memory" is re-
      //  leased, that memory gets wiped out immediately.
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_CALLOC);
      return nullptr;
    }

    // 12.3 MPI formats, Pg 75
    err = gcry_mpi_print(GCRYMPI_FMT_USG, mpibuf, MPIBUFLEN, NULL, mpi);
    //  Convert the MPI 'a' into an external representation described by 'format'
    //  ... and store it in the provided buffer which has a usable length of at
    //  least the 'buflen' bytes. If 'nwritten' is not NULL, it will receive the
    //  number of bytes actually stored in 'buffer' after a successful operation.
  
    // 12.2 Basic functions, Pg 73
    gcry_mpi_release(mpi);
    //  Release the MPI 'a' and free all associated resources. Passing NULL is
    //  allowed and ignored. When a MPI stored in the "secure memory" is re-
    //  leased, that memory gets wiped out immediately.

    return (err ? nullptr : mpibuf); 
  }

  /******************************************************************************
   *
   */
  bool srvcry::hash_text (std::string& hashcode, std::string const& text) {
    std::string hash = m_hashkey;

    for (size_t i = 0; i < text.size(); ++i)
      hash[i] = text[i];

    // 7.2 Working with hash algorithms, Pg 52
    gcry_error_t err = 0;
    gcry_md_hd_t hash_hd;
    err = gcry_md_open(&hash_hd, GCRY_MD_SHA256, GCRY_MD_FLAG_SECURE);
    //  Create a message digest object for algorithm 'algo'. 'flags' may be
    //  given as an bitwise OR of constants described below. 'algo' may be
    //  given as 0 if the algorithms to use are later set using gcry_md_enable.
    //  'hd' is guaranteed to either receive a valid handle or NULL.
    //  The flags allowed for mode are:
    //    GCRY_MD_FLAG_SECURE
    //    Allocate all buffers and the resulting digest in "secure memory". Use
    //    this is the hashed data is highly confidential.
    //  7.1 Available hash algorithms, Pg 50
    //    GCRY_MD_SHA256
    //    This is the SHA-256 algorithm which yields a message digest of 32
    //    bytes. See FIPS 180-2 for the specification.
    if (err) return false;

    // 7.2 Working with hash algorithms, Pg 53
    // ... There are two ways for this, one to update the hash with a block of
    // memory and one macro to update the hash by just one character. Both
    // methods can be used on the same hash context.
    gcry_md_write(hash_hd,
        reinterpret_cast<void const*>(hash.c_str()), hash.size());
    //  Pass 'length' bytes of the data in 'buffer' to the digest object with
    //  handle 'h' to update the digest values. This function should be used
    //  for large blocks of data. If this function is used after the context
    //  has been finalized, it will keep on pushing the data through the
    //  algorithm specific transform function and change the context; however
    //  the results are not meaningful and this feature is only available to
    //  mitigate timing attacks.

    // 7.2 Working with hash algorithms, Pg 54
    // The way to read out the calculated message digest is by using the func-
    // tion:
    uint8_t* encbuf;
    encbuf = gcry_md_read(hash_hd, GCRY_MD_SHA256);
    //  gcry_md_read returns the message digest after finalizing the calculation.
    //  This function may be used as often as required but it will always return
    //  the same value for one handle. The returned message digest is allocated
    //  within the message context and therefore valid until the handle is
    //  released or reseted (using gcry_md_close or gcry_md_reset or it has
    //  been updated as a mitigation measure against timing attacks. 'algo' may
    //  be given as 0 to return the only enabled message digest or it may
    //  specify one of the enabled algorithms. The function does return NULL if
    //  the requested algorithm has not been enabled.
    if (encbuf == nullptr) {
      gcry_md_close(hash_hd);
      return false;
    }

    // 7.2 Working with hash algorithms, Pg 54
    // If the length of a message digest is not known, it can be retrieved
    // using the following function:
    uint32_t md_len = gcry_md_get_algo_dlen(GCRY_MD_SHA256);
    //  Retrieve the length in bytes of the digest yielded by algorithm 'algo'.
    //  This is often used prior to gcry_md_read to allocate sufficient memory
    //  for the digest.
    for (uint32_t i = 0; i < md_len; i++)
    {
      char ch [8];
      sprintf(ch, "%02X", static_cast<uint8_t>(encbuf[i]));
      hashcode += ch;
    }

    // After you are done with the hash calculation, you should release the
    // resources by using:
    gcry_md_close(hash_hd);
    //  Release all resources of hash context 'h'. 'h' should not be used after
    //  a call to this function. A NULL passed as 'h' is ignored. The function
    //  also zeroises all sensitive information associated with this handle.

    return true;
  }

  /****************************************************************************
   *
   */
  bool srvcry::hashgen () {
    time_t tim = std::chrono::system_clock::to_time_t(
        std::chrono::high_resolution_clock::now());
    size_t size = static_cast<size_t>(tim) % 128L;
    for (size_t i = 0; i < size; ++i) (void) rand_int32(MIN_ZERO, MAX_RAND);
    std::string hashcode;
    while (hashcode.size() < MAXHASH) {
      int32_t ri = rand_int32(MIN_ZERO, MAX_RAND) % MAX_RANGE;
      hashcode += static_cast<char>(RANGE_BASE + ri);
    }
    std::ostringstream path;
    path << m_base_path << '/' << m_hash_name;
    std::ofstream fs;
    fs.open(path.str(), (fs.trunc | fs.out));
    if (not fs.is_open()) {
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_HASH_FILE);
      return false;
    }
    fs << hashcode;
    fs.flush();
    return true;
  }

  /****************************************************************************
   *
   */
  bool srvcry::keygen () {
    // The Libgcrypt Reference Manual, v1.8.2 23, Nov. 2017

    if (not is_initialized()) return false;

    // initialize an AES handle (context)
    gcry_cipher_hd_t aes_hd;
    if (not init_aes_hd(&aes_hd)) return false;

    gcry_error_t err = 0;

    // 11.2 Working with S-expressions, Pg 67-68
    // There are several functions to create an Libgcrypt S-expression object
    // from its external representation or from a string template.
    gcry_sexp_t sexp;
    err = gcry_sexp_build(&sexp, nullptr, "(genkey (rsa (nbits 4:4096)))");
    //  This function creates an internal S-expression from the string template
    //  format and stores it at the address of r sexp. If there is a parsing
    //  error, the function returns an appropriate error code and stores the
    //  offset into format where the parsing stopped in erroff. The function
    //  supports a couple of printf-like formatting characters and expects
    //  arguments for some of these escape sequences right after format.
    if (err) {
      // 5.3 Working with cipher handles, Pg 29
      // Use the following function to release an existing handle:
      gcry_cipher_close(aes_hd);
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_SEXP_BUILD);
      return false;
    }

    // 6.4 General public-key related Functions, Pg 44
    // Libgcrypt also provides a function to generate public key pairs:
    gcry_sexp_t keypair;
    err = gcry_pk_genkey(&keypair, sexp);
    //  This function create a new public key pair using information given in
    //  the S-expression 'sexp' and stores the private and the public key in
    //  one new S-expression at the address given by 'keypair'. In case of an
    //  error, 'keypair'is set to nullptr. The return code is 0 for success or an
    //  error code otherwise. Here is an example for 'sexp' to create an 2048
    //  bit RSA key: (genkey (rsa (nbits 4:2048)))
    if (err) {
      // 11.2 Working with S-expressions, Pg 68
      gcry_sexp_release(sexp);
      // 5.3 Working with cipher handles, Pg 29
      // Use the following function to release an existing handle:
      gcry_cipher_close(aes_hd);
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_PK_GENKEY);
      return false;
    }

    // allocate keypair buffer
    void* buffer = calloc(1L, BUFLEN);
    if (buffer == nullptr) {
      // 11.2 Working with S-expressions, Pg 68
      gcry_sexp_release(keypair);
      gcry_sexp_release(sexp);
      // 5.3 Working with cipher handles, Pg 29
      // Use the following function to release an existing handle:
      gcry_cipher_close(aes_hd);
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_CALLOC);
      return false;
    }

    // 11.2 Working with S-expressions, Pg 68-69
    // ... used to convert the internal representation back into a regular
    // external S-expression format and to show the structure for debugging.
    (void) gcry_sexp_sprint(keypair, GCRYSEXP_FMT_CANON, buffer, BUFLEN);

    // 5.3 Working with cipher handles, Pg 30
    // To use a cipher algorithm, you must first allocate an according handle.
    // This is to be done using the open function:
    err = gcry_cipher_encrypt(aes_hd, reinterpret_cast<uint8_t*>(buffer),
        BUFLEN, nullptr, 0L);
    //  ... This function can either work in place or with two buffers. It uses
    //  the cipher context already setup and described by the handle 'h'. ...
    //  If 'in' is passed as NULL and 'inlen' is 0, in-place encryption of the
    //  data in 'out' of length 'outsize' takes place.
    if (err) {
      free(buffer);
      // 11.2 Working with S-expressions, Pg 68
      gcry_sexp_release(keypair);
      gcry_sexp_release(sexp);
      // 5.3 Working with cipher handles, Pg 29
      // Use the following function to release an existing handle:
      gcry_cipher_close(aes_hd);
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_CIPHER_ENCRYPT);
      return false;
    }

    // construct keypair path
    std::ostringstream path;
    path << m_base_path << '/' << m_keyp_name;

    // create keypair file
    std::ofstream fs;
    fs.open(path.str(), (fs.binary | fs.trunc | fs.out));
    if (fs.is_open()) {
      // write keypair file
      fs.clear();
      fs.write(reinterpret_cast<char const*>(buffer), BUFLEN);
      fs.flush();
    } else report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_KEYP_FILE);

    free(buffer);
    // 11.2 Working with S-expressions, Pg 68
    gcry_sexp_release(keypair);
    gcry_sexp_release(sexp);
    //  Release the S-expression object sexp. If the S-expression is stored in
    //  secure memory it explicitly zeroises that memory; note that this is
    //  done in addition to the zeroisation always done when freeing secure
    //  memory.

    // 5.3 Working with cipher handles, Pg 29
    // Use the following function to release an existing handle:
    gcry_cipher_close(aes_hd);

    return true;
  }

  /****************************************************************************
   *
   */
  bool srvcry::load_hash () {
    std::ostringstream path;
    path << m_base_path << '/' << m_hash_name;
    std::ifstream fs;
    fs.open(path.str());
    if (not fs.is_open()) {
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_HASH_FILE);
      return false;
    }
    fs >> m_hashkey;
    return true;
  }

  /****************************************************************************
   *
   */
  bool srvcry::load_keypair () {
    // The Libgcrypt Reference Manual, v1.8.2 23, Nov. 2017

    if (not is_initialized()) return false;

    // construct keypair path
    std::ostringstream path;
    path << m_base_path << '/' << m_keyp_name;

    std::ifstream fs;
    fs.open(path.str(), (fs.binary | fs.in));
    if (not fs.is_open()) {
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_KEYP_FILE);
      return false;
    }

    void* buffer = calloc(1, BUFLEN);
    if (buffer == nullptr) {
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_CALLOC);
      return false;
    }

    fs.clear();
    fs.read(reinterpret_cast<char*>(buffer), BUFLEN);
    if (not fs) { free(buffer); return false; }

    // initialize an AES handle (context)
    gcry_cipher_hd_t aes_hd;
    if (not init_aes_hd(&aes_hd)) { free(buffer); return false; }

    gcry_error_t err = 0;

    // 5.3 Working with cipher handles, Pg 31
    err = gcry_cipher_decrypt(aes_hd, reinterpret_cast<uint8_t*>(buffer),
        BUFLEN, NULL, 0);
    //  ... This function can either work in place or with two buffers. It uses
    //  the cipher context already setup and described by the handle 'h'. ...
    //  If 'in' is passed as NULL and 'inlen' is 0, in-place decryption of the
    //  data in 'out' or length 'outsize' takes place. The function returns 0
    //  on success or an error code.
    if (err) {
      // 5.3 Working with cipher handles, Pg 29
      // Use the following function to release an existing handle:
      gcry_cipher_close(aes_hd);
      free(buffer);
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_CIPHER_DECRYPT);
      return false;
    }

    // 11.2 Working with S-expressions, Pg 67
    gcry_sexp_t keypair;
    err = gcry_sexp_new(&keypair, buffer, BUFLEN, 0);
    //  This is the generic function to create an new S-expression object from
    //  its external representation in buffer of length bytes. On success the
    //  result is stored at the address given by 'r_sexp'. With 'autodetect'
    //  set to 0, the data in 'buffer' is expected to be in canonized format,
    //  with 'autodetect' set to 1 the parses any of the defined external for-
    //  mats. If 'buffer' does not hold a valid S-expression an error code is
    //  returned and 'r_sexp' set to NULL. Note that the caller is responsible
    //  for releasing the newly allocated S-expression using gcry_sexp_release.
    if (err) {
      // 5.3 Working with cipher handles, Pg 29
      // Use the following function to release an existing handle:
      gcry_cipher_close(aes_hd);
      free(buffer);
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_CIPHER_DECRYPT);
      return false;
    }

    // 11.2 Working with S-expressions, Pg 69
    // There are functions to parse S-expressions and retrieve elements:
    m_prikey = gcry_sexp_find_token(keypair, "private-key", 0);
    m_pubkey = gcry_sexp_find_token(keypair, "public-key", 0);
    //  Scan the S-expression for a sublist with a type (the car of the list)
    //  matching the string 'token'. If 'toklen' is not 0, the 'token' is
    //  assumed to be raw memory of this length. The function returns a newly
    //  allocated S-expression consisting of the found sublist or NULL (?) when
    //  not found.

    // 11.2 Working with S-expressions, Pg 68
    gcry_sexp_release(keypair);
    //  Release the S-expression object sexp. If the S-expression is stored in
    //  secure memory it explicitly zeroises that memory; note that this is
    //  done in addition to the zeroisation always done when freeing secure
    //  memory.

    // 5.3 Working with cipher handles, Pg 29
    // Use the following function to release an existing handle:
    gcry_cipher_close(aes_hd);
    free(buffer);

    return true;
  }

  /****************************************************************************
   *
   */
  bool srvcry::pubkey_to_buffer(size_t* buflen, uint8_t* buffer) {
    // The Libgcrypt Reference Manual, v1.8.2 23, Nov. 2017

    if (not is_initialized()) return false;

    // 11.2 Working with S-expressions, Pg 68-69
    // ... used to convert the internal representation back into a regular
    // external S-expression format and to show the structure for debugging.
    *buflen = gcry_sexp_sprint(m_pubkey, GCRYSEXP_FMT_CANON, buffer, *buflen);
    //  Copies the S-expression object 'sexp' into 'buffer' using the format
    //  specified in 'mode'. 'maxlength' must be set to the allocated length
    //  of 'buffer'. The function returns the actual length of valid bytes put
    //  into 'buffer' or 0 if the provided 'buffer' is too short. Passing NULL
    //  for 'buffer' returns the required length for 'buffer'. For convenience
    //  reasons an extra byte with value 0 is appended to the 'buffer'.
    //  The following formats are supported:
    //    GCRYSEXP_FMT_CANON
    //    Return the S-expression in canonical format.

    return true;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS GCRY: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  std::string srvcry::get_password () {
    std::string password;

    std::ostringstream path;
    path << m_base_path << '/' << m_pkey_name;

    std::fstream fs;
    fs.open(path.str(), fs.in);
    if (fs.is_open()) {
      fs >> password;
    } else {
      fs.clear();
      fs.open(path.str(), fs.out);
      if (not fs.is_open()) return password;
      password = generate_password();
      fs << password;
      fs.flush();
    }

    return password;
  }

  /****************************************************************************
   *
   */
  void srvcry::init()
  {
    // The Libgcrypt Reference Manual, v1.8.2 23, Nov. 2017

    // 2.4 Initializing the library, Pg 4
    // If you have to protect your keys or other information in memory against
    // being swapped out to disk and to enable an automatic overwrite of used
    // and freed memory, you need to initialize Libgcrypt this way:
    if (gcry_check_version (GCRYPT_VERSION) == nullptr) {
      m_is_initialized = false;
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_LIB_INIT);
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

    if (err) {
      m_is_initialized = false;
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_LIB_INIT);
      return;
    }
  }
  
  /****************************************************************************
   *
   */
  bool srvcry::init_aes_hd(gcry_cipher_hd_t* aes_hd)
  {
    // generator password
    std::string password = get_password();

    // The Libgcrypt Reference Manual, v1.8.2 23, Nov. 2017

    if (not is_initialized()) return false;

    // 5.3 Working with cipher handles, Pg 28
    // To use a cipher algorithm, you must first allocate an according handle.
    // This is to be done using the open function:
    int err = gcry_cipher_open(aes_hd, GCRY_CIPHER_AES128, 
        GCRY_CIPHER_MODE_CFB, 0);
    //  This function creates the context handle required for most of the other
    //  cipher functions and returns a handle to it in 'aes_hd'. In case of an
    //  error, an according error code is returned.
    if (err) {
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_CIPHER_OPEN);
      return false;
    }

    // 7.2 Working with hash algorithms, Pg 55
    // Because it is often necessary to get the message digest of blocks of
    // memory, two fast convenience function are available for this task:
    size_t const diglen = static_cast<size_t>(128 / 8);
    char digkey[diglen];
    gcry_md_hash_buffer(GCRY_MD_MD5, reinterpret_cast<void*>(&digkey),
        reinterpret_cast<void const*>(password.c_str()), password.size());
    //  ... This function does not require a context and immediately returns
    //  the message digest of the 'length' bytes at 'buffer'. 'digest' must be
    //  allocated by the caller, large enough to hold the message digest
    //  yielded by the the specified algorithm 'algo'.

    // 5.3 Working with cipher handles, Pg 29
    // In order to use a handle for performing cryptographic operations, a
    // 'key' has to be set first:
    err = gcry_cipher_setkey(*aes_hd, reinterpret_cast<void const*>(&digkey),
        diglen);
    //  Set the key 'digkey' used for encryption or decryption in the context
    //  denoted by the handle 'ase_hd'. The length 'diglen' (in bytes) of the
    //  key 'digkey' must match the required length of the algorithm set for
    //  this context or be in the allowed range for algorithms with variable
    //  key size. The function checks this and returns an error if there is a
    //  problem. A caller should always check for an error.
    if (err) {
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_CIPHER_SETKEY);
      return false;
    }

    // Most crypto modes requires an initialization vector (IV), which usually
    // is a non-secret random string acting as a kind of salt value. The CTR
    // mode requires a counter, which is also similar to a salt value. To set
    // the IV or CTR, use these functions:
    err = gcry_cipher_setiv(*aes_hd,
        reinterpret_cast<void const*>(&digkey), diglen);
    //  Set the initialization vector used for encryption or decryption. The
    //  vector is passed as the buffer 'digkey' length 'diglen' bytes and
    //  copied to internal data structures. The function checks that the IV
    //  matches the requirement of the selected algorithm and mode.
    if (err) {
      report(m_logger, eno::ERRNO_CRYPTO_ERROR, GCRY_CIPHER_SETIV);
      return false;
    }

    return true;
  }

} // namespace

