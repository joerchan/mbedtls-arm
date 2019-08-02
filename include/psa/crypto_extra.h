/**
 * \file psa/crypto_extra.h
 *
 * \brief PSA cryptography module: Mbed TLS vendor extensions
 *
 * \note This file may not be included directly. Applications must
 * include psa/crypto.h.
 *
 * This file is reserved for vendor-specific definitions.
 */
/*
 *  Copyright (C) 2018, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#ifndef PSA_CRYPTO_EXTRA_H
#define PSA_CRYPTO_EXTRA_H

#include "mbedtls/platform_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* UID for secure storage seed */
#define PSA_CRYPTO_ITS_RANDOM_SEED_UID 0xFFFFFF52

/*
 * Deprecated PSA Crypto error code definitions
 */
#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#define PSA_ERROR_UNKNOWN_ERROR \
    MBEDTLS_DEPRECATED_NUMERIC_CONSTANT( PSA_ERROR_GENERIC_ERROR )
#endif

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#define PSA_ERROR_OCCUPIED_SLOT \
    MBEDTLS_DEPRECATED_NUMERIC_CONSTANT( PSA_ERROR_ALREADY_EXISTS )
#endif

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#define PSA_ERROR_EMPTY_SLOT \
    MBEDTLS_DEPRECATED_NUMERIC_CONSTANT( PSA_ERROR_DOES_NOT_EXIST )
#endif

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#define PSA_ERROR_INSUFFICIENT_CAPACITY \
    MBEDTLS_DEPRECATED_NUMERIC_CONSTANT( PSA_ERROR_INSUFFICIENT_DATA )
#endif

/** \addtogroup attributes
 * @{
 */

/** \brief Declare the enrollment algorithm for a key.
 *
 * An operation on a key may indifferently use the algorithm set with
 * psa_set_key_algorithm() or with this function.
 *
 * \param[out] attributes       The attribute structure to write to.
 * \param alg2                  A second algorithm that the key may be used
 *                              for, in addition to the algorithm set with
 *                              psa_set_key_algorithm().
 *
 * \warning Setting an enrollment algorithm is not recommended, because
 *          using the same key with different algorithms can allow some
 *          attacks based on arithmetic relations between different
 *          computations made with the same key, or can escalate harmless
 *          side channels into exploitable ones. Use this function only
 *          if it is necessary to support a protocol for which it has been
 *          verified that the usage of the key with multiple algorithms
 *          is safe.
 */
static inline void psa_set_key_enrollment_algorithm(
    psa_key_attributes_t *attributes,
    psa_algorithm_t alg2)
{
    attributes->core.policy.alg2 = alg2;
}

/** Retrieve the enrollment algorithm policy from key attributes.
 *
 * \param[in] attributes        The key attribute structure to query.
 *
 * \return The enrollment algorithm stored in the attribute structure.
 */
static inline psa_algorithm_t psa_get_key_enrollment_algorithm(
    const psa_key_attributes_t *attributes)
{
    return( attributes->core.policy.alg2 );
}

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)

/** Retrieve the slot number where a key is stored.
 *
 * A slot number is only defined for keys that are stored in a secure
 * element.
 *
 * This information is only useful if the secure element is not entirely
 * managed through the PSA Cryptography API. It is up to the secure
 * element driver to decide how PSA slot numbers map to any other interface
 * that the secure element may have.
 *
 * \param[in] attributes        The key attribute structure to query.
 * \param[out] slot_number      On success, the slot number containing the key.
 *
 * \retval #PSA_SUCCESS
 *         The key is located in a secure element, and \p *slot_number
 *         indicates the slot number that contains it.
 * \retval #PSA_ERROR_NOT_PERMITTED
 *         The caller is not permitted to query the slot number.
 *         Mbed Crypto currently does not return this error.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The key is not located in a secure element.
 */
psa_status_t psa_get_key_slot_number(
    const psa_key_attributes_t *attributes,
    psa_key_slot_number_t *slot_number );

/** Choose the slot number where a key is stored.
 *
 * This function declares a slot number in the specified attribute
 * structure.
 *
 * A slot number is only meaningful for keys that are stored in a secure
 * element. It is up to the secure element driver to decide how PSA slot
 * numbers map to any other interface that the secure element may have.
 *
 * \note Setting a slot number in key attributes for a key creation can
 *       cause the following errors when creating the key:
 *       - #PSA_ERROR_NOT_SUPPORTED if the selected secure element does
 *         not support choosing a specific slot number.
 *       - #PSA_ERROR_NOT_PERMITTED if the caller is not permitted to
 *         choose slot numbers in general or to choose this specific slot.
 *       - #PSA_ERROR_INVALID_ARGUMENT if the chosen slot number is not
 *         valid in general or not valid for this specific key.
 *       - #PSA_ERROR_ALREADY_EXISTS if there is already a key in the
 *         selected slot.
 *
 * \param[out] attributes       The attribute structure to write to.
 * \param slot_number           The slot number to set.
 */
static inline void psa_set_key_slot_number(
    psa_key_attributes_t *attributes,
    psa_key_slot_number_t slot_number )
{
    attributes->core.flags |= MBEDTLS_PSA_KA_FLAG_HAS_SLOT_NUMBER;
    attributes->slot_number = slot_number;
}

/** Remove the slot number attribute from a key attribute structure.
 *
 * This function undoes the action of psa_set_key_slot_number().
 *
 * \param[out] attributes       The attribute structure to write to.
 */
static inline void psa_clear_key_slot_number(
    psa_key_attributes_t *attributes )
{
    attributes->core.flags &= ~MBEDTLS_PSA_KA_FLAG_HAS_SLOT_NUMBER;
}

#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

/**@}*/

/**
 * \brief Library deinitialization.
 *
 * This function clears all data associated with the PSA layer,
 * including the whole key store.
 *
 * This is an Mbed TLS extension.
 */
void mbedtls_psa_crypto_free( void );

/** \brief Statistics about
 * resource consumption related to the PSA keystore.
 *
 * \note The content of this structure is not part of the stable API and ABI
 *       of Mbed Crypto and may change arbitrarily from version to version.
 */
typedef struct mbedtls_psa_stats_s
{
    /** Number of slots containing key material for a volatile key. */
    size_t volatile_slots;
    /** Number of slots containing key material for a key which is in
     * internal persistent storage. */
    size_t persistent_slots;
    /** Number of slots containing a reference to a key in a
     * secure element. */
    size_t external_slots;
    /** Number of slots which are occupied, but do not contain
     * key material yet. */
    size_t half_filled_slots;
    /** Number of slots that contain cache data. */
    size_t cache_slots;
    /** Number of slots that are not used for anything. */
    size_t empty_slots;
    /** Largest key id value among open keys in internal persistent storage. */
    psa_key_id_t max_open_internal_key_id;
    /** Largest key id value among open keys in secure elements. */
    psa_key_id_t max_open_external_key_id;
} mbedtls_psa_stats_t;

/** \brief Get statistics about
 * resource consumption related to the PSA keystore.
 *
 * \note When Mbed Crypto is built as part of a service, with isolation
 *       between the application and the keystore, the service may or
 *       may not expose this function.
 */
void mbedtls_psa_get_stats( mbedtls_psa_stats_t *stats );

/**
 * \brief Inject an initial entropy seed for the random generator into
 *        secure storage.
 *
 * This function injects data to be used as a seed for the random generator
 * used by the PSA Crypto implementation. On devices that lack a trusted
 * entropy source (preferably a hardware random number generator),
 * the Mbed PSA Crypto implementation uses this value to seed its
 * random generator.
 *
 * On devices without a trusted entropy source, this function must be
 * called exactly once in the lifetime of the device. On devices with
 * a trusted entropy source, calling this function is optional.
 * In all cases, this function may only be called before calling any
 * other function in the PSA Crypto API, including psa_crypto_init().
 *
 * When this function returns successfully, it populates a file in
 * persistent storage. Once the file has been created, this function
 * can no longer succeed.
 *
 * If any error occurs, this function does not change the system state.
 * You can call this function again after correcting the reason for the
 * error if possible.
 *
 * \warning This function **can** fail! Callers MUST check the return status.
 *
 * \warning If you use this function, you should use it as part of a
 *          factory provisioning process. The value of the injected seed
 *          is critical to the security of the device. It must be
 *          *secret*, *unpredictable* and (statistically) *unique per device*.
 *          You should be generate it randomly using a cryptographically
 *          secure random generator seeded from trusted entropy sources.
 *          You should transmit it securely to the device and ensure
 *          that its value is not leaked or stored anywhere beyond the
 *          needs of transmitting it from the point of generation to
 *          the call of this function, and erase all copies of the value
 *          once this function returns.
 *
 * This is an Mbed TLS extension.
 *
 * \note This function is only available on the following platforms:
 * * If the compile-time option MBEDTLS_PSA_INJECT_ENTROPY is enabled.
 *   Note that you must provide compatible implementations of
 *   mbedtls_nv_seed_read and mbedtls_nv_seed_write.
 * * In a client-server integration of PSA Cryptography, on the client side,
 *   if the server supports this feature.
 * \param[in] seed          Buffer containing the seed value to inject.
 * \param[in] seed_size     Size of the \p seed buffer.
 *                          The size of the seed in bytes must be greater
 *                          or equal to both #MBEDTLS_ENTROPY_MIN_PLATFORM
 *                          and #MBEDTLS_ENTROPY_BLOCK_SIZE.
 *                          It must be less or equal to
 *                          #MBEDTLS_ENTROPY_MAX_SEED_SIZE.
 *
 * \retval #PSA_SUCCESS
 *         The seed value was injected successfully. The random generator
 *         of the PSA Crypto implementation is now ready for use.
 *         You may now call psa_crypto_init() and use the PSA Crypto
 *         implementation.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \p seed_size is out of range.
 * \retval #PSA_ERROR_STORAGE_FAILURE
 *         There was a failure reading or writing from storage.
 * \retval #PSA_ERROR_NOT_PERMITTED
 *         The library has already been initialized. It is no longer
 *         possible to call this function.
 */
psa_status_t mbedtls_psa_inject_entropy(uint8_t *seed,
                                        size_t seed_size);

#if defined(PSA_PRE_1_0_KEY_DERIVATION)
/** Set up a key derivation operation.
 *
 * FIMXE This function is no longer part of the official API. Its prototype
 * is only kept around for the sake of tests that haven't been updated yet.
 *
 * A key derivation algorithm takes three inputs: a secret input \p handle and
 * two non-secret inputs \p label and p salt.
 * The result of this function is a byte generator which can
 * be used to produce keys and other cryptographic material.
 *
 * The role of \p label and \p salt is as follows:
 * - For HKDF (#PSA_ALG_HKDF), \p salt is the salt used in the "extract" step
 *   and \p label is the info string used in the "expand" step.
 *
 * \param[in,out] operation       The key derivation object to set up. It must
 *                                have been initialized as per the documentation
 *                                for #psa_key_derivation_operation_t and not
 *                                yet be in use.
 * \param handle                  Handle to the secret key.
 * \param alg                     The key derivation algorithm to compute
 *                                (\c PSA_ALG_XXX value such that
 *                                #PSA_ALG_IS_KEY_DERIVATION(\p alg) is true).
 * \param[in] salt                Salt to use.
 * \param salt_length             Size of the \p salt buffer in bytes.
 * \param[in] label               Label to use.
 * \param label_length            Size of the \p label buffer in bytes.
 * \param capacity                The maximum number of bytes that the
 *                                operation will be able to provide.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INVALID_HANDLE
 * \retval #PSA_ERROR_EMPTY_SLOT
 * \retval #PSA_ERROR_NOT_PERMITTED
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \c key is not compatible with \c alg,
 *         or \p capacity is too large for the specified algorithm and key.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         \c alg is not supported or is not a key derivation algorithm.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE
 * \retval #PSA_ERROR_HARDWARE_FAILURE
 * \retval #PSA_ERROR_CORRUPTION_DETECTED
 * \retval #PSA_ERROR_BAD_STATE
 *         The library has not been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t psa_key_derivation(psa_key_derivation_operation_t *operation,
                                psa_key_handle_t handle,
                                psa_algorithm_t alg,
                                const uint8_t *salt,
                                size_t salt_length,
                                const uint8_t *label,
                                size_t label_length,
                                size_t capacity);
#endif /* PSA_PRE_1_0_KEY_DERIVATION */

/** \addtogroup crypto_types
 * @{
 */

/** DSA public key.
 *
 * The import and export format is the
 * representation of the public key `y = g^x mod p` as a big-endian byte
 * string. The length of the byte string is the length of the base prime `p`
 * in bytes.
 */
#define PSA_KEY_TYPE_DSA_PUBLIC_KEY             ((psa_key_type_t)0x60020000)

/** DSA key pair (private and public key).
 *
 * The import and export format is the
 * representation of the private key `x` as a big-endian byte string. The
 * length of the byte string is the private key size in bytes (leading zeroes
 * are not stripped).
 *
 * Determinstic DSA key derivation with psa_generate_derived_key follows
 * FIPS 186-4 &sect;B.1.2: interpret the byte string as integer
 * in big-endian order. Discard it if it is not in the range
 * [0, *N* - 2] where *N* is the boundary of the private key domain
 * (the prime *p* for Diffie-Hellman, the subprime *q* for DSA,
 * or the order of the curve's base point for ECC).
 * Add 1 to the resulting integer and use this as the private key *x*.
 *
 */
#define PSA_KEY_TYPE_DSA_KEY_PAIR                ((psa_key_type_t)0x70020000)

/** Whether a key type is an DSA key (pair or public-only). */
#define PSA_KEY_TYPE_IS_DSA(type)                                       \
    (PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) == PSA_KEY_TYPE_DSA_PUBLIC_KEY)

#define PSA_ALG_DSA_BASE                        ((psa_algorithm_t)0x10040000)
/** DSA signature with hashing.
 *
 * This is the signature scheme defined by FIPS 186-4,
 * with a random per-message secret number (*k*).
 *
 * \param hash_alg      A hash algorithm (\c PSA_ALG_XXX value such that
 *                      #PSA_ALG_IS_HASH(\p hash_alg) is true).
 *                      This includes #PSA_ALG_ANY_HASH
 *                      when specifying the algorithm in a usage policy.
 *
 * \return              The corresponding DSA signature algorithm.
 * \return              Unspecified if \p hash_alg is not a supported
 *                      hash algorithm.
 */
#define PSA_ALG_DSA(hash_alg)                             \
    (PSA_ALG_DSA_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_DETERMINISTIC_DSA_BASE          ((psa_algorithm_t)0x10050000)
#define PSA_ALG_DSA_DETERMINISTIC_FLAG          ((psa_algorithm_t)0x00010000)
/** Deterministic DSA signature with hashing.
 *
 * This is the deterministic variant defined by RFC 6979 of
 * the signature scheme defined by FIPS 186-4.
 *
 * \param hash_alg      A hash algorithm (\c PSA_ALG_XXX value such that
 *                      #PSA_ALG_IS_HASH(\p hash_alg) is true).
 *                      This includes #PSA_ALG_ANY_HASH
 *                      when specifying the algorithm in a usage policy.
 *
 * \return              The corresponding DSA signature algorithm.
 * \return              Unspecified if \p hash_alg is not a supported
 *                      hash algorithm.
 */
#define PSA_ALG_DETERMINISTIC_DSA(hash_alg)                             \
    (PSA_ALG_DETERMINISTIC_DSA_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_IS_DSA(alg)                                             \
    (((alg) & ~PSA_ALG_HASH_MASK & ~PSA_ALG_DSA_DETERMINISTIC_FLAG) ==  \
     PSA_ALG_DSA_BASE)
#define PSA_ALG_DSA_IS_DETERMINISTIC(alg)               \
    (((alg) & PSA_ALG_DSA_DETERMINISTIC_FLAG) != 0)
#define PSA_ALG_IS_DETERMINISTIC_DSA(alg)                       \
    (PSA_ALG_IS_DSA(alg) && PSA_ALG_DSA_IS_DETERMINISTIC(alg))
#define PSA_ALG_IS_RANDOMIZED_DSA(alg)                          \
    (PSA_ALG_IS_DSA(alg) && !PSA_ALG_DSA_IS_DETERMINISTIC(alg))


/* We need to expand the sample definition of this macro from
 * the API definition. */
#undef PSA_ALG_IS_HASH_AND_SIGN
#define PSA_ALG_IS_HASH_AND_SIGN(alg)                                   \
    (PSA_ALG_IS_RSA_PSS(alg) || PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) ||    \
     PSA_ALG_IS_DSA(alg) || PSA_ALG_IS_ECDSA(alg))

/**@}*/

/** \addtogroup attributes
 * @{
 */

/** Custom Diffie-Hellman group.
 *
 * For keys of type #PSA_KEY_TYPE_DH_PUBLIC_KEY(#PSA_DH_GROUP_CUSTOM) or
 * #PSA_KEY_TYPE_DH_KEY_PAIR(#PSA_DH_GROUP_CUSTOM), the group data comes
 * from domain parameters set by psa_set_key_domain_parameters().
 */
/* This value is reserved for private use in the TLS named group registry. */
#define PSA_DH_GROUP_CUSTOM             ((psa_dh_group_t) 0x01fc)


/**
 * \brief Set domain parameters for a key.
 *
 * Some key types require additional domain parameters in addition to
 * the key type identifier and the key size. Use this function instead
 * of psa_set_key_type() when you need to specify domain parameters.
 *
 * The format for the required domain parameters varies based on the key type.
 *
 * - For RSA keys (#PSA_KEY_TYPE_RSA_PUBLIC_KEY or #PSA_KEY_TYPE_RSA_KEY_PAIR),
 *   the domain parameter data consists of the public exponent,
 *   represented as a big-endian integer with no leading zeros.
 *   This information is used when generating an RSA key pair.
 *   When importing a key, the public exponent is read from the imported
 *   key data and the exponent recorded in the attribute structure is ignored.
 *   As an exception, the public exponent 65537 is represented by an empty
 *   byte string.
 * - For DSA keys (#PSA_KEY_TYPE_DSA_PUBLIC_KEY or #PSA_KEY_TYPE_DSA_KEY_PAIR),
 *   the `Dss-Parms` format as defined by RFC 3279 &sect;2.3.2.
 *   ```
 *   Dss-Parms ::= SEQUENCE  {
 *      p       INTEGER,
 *      q       INTEGER,
 *      g       INTEGER
 *   }
 *   ```
 * - For Diffie-Hellman key exchange keys
 *   (#PSA_KEY_TYPE_DH_PUBLIC_KEY(#PSA_DH_GROUP_CUSTOM) or
 *   #PSA_KEY_TYPE_DH_KEY_PAIR(#PSA_DH_GROUP_CUSTOM)), the
 *   `DomainParameters` format as defined by RFC 3279 &sect;2.3.3.
 *   ```
 *   DomainParameters ::= SEQUENCE {
 *      p               INTEGER,                    -- odd prime, p=jq +1
 *      g               INTEGER,                    -- generator, g
 *      q               INTEGER,                    -- factor of p-1
 *      j               INTEGER OPTIONAL,           -- subgroup factor
 *      validationParms ValidationParms OPTIONAL
 *   }
 *   ValidationParms ::= SEQUENCE {
 *      seed            BIT STRING,
 *      pgenCounter     INTEGER
 *   }
 *   ```
 *
 * \note This function may allocate memory or other resources.
 *       Once you have called this function on an attribute structure,
 *       you must call psa_reset_key_attributes() to free these resources.
 *
 * \note This is an experimental extension to the interface. It may change
 *       in future versions of the library.
 *
 * \param[in,out] attributes    Attribute structure where the specified domain
 *                              parameters will be stored.
 *                              If this function fails, the content of
 *                              \p attributes is not modified.
 * \param type                  Key type (a \c PSA_KEY_TYPE_XXX value).
 * \param[in] data              Buffer containing the key domain parameters.
 *                              The content of this buffer is interpreted
 *                              according to \p type as described above.
 * \param data_length           Size of the \p data buffer in bytes.
 *
 * \retval #PSA_SUCCESS
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 * \retval #PSA_ERROR_NOT_SUPPORTED
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY
 */
psa_status_t psa_set_key_domain_parameters(psa_key_attributes_t *attributes,
                                           psa_key_type_t type,
                                           const uint8_t *data,
                                           size_t data_length);

/**
 * \brief Get domain parameters for a key.
 *
 * Get the domain parameters for a key with this function, if any. The format
 * of the domain parameters written to \p data is specified in the
 * documentation for psa_set_key_domain_parameters().
 *
 * \note This is an experimental extension to the interface. It may change
 *       in future versions of the library.
 *
 * \param[in] attributes        The key attribute structure to query.
 * \param[out] data             On success, the key domain parameters.
 * \param data_size             Size of the \p data buffer in bytes.
 *                              The buffer is guaranteed to be large
 *                              enough if its size in bytes is at least
 *                              the value given by
 *                              PSA_KEY_DOMAIN_PARAMETERS_SIZE().
 * \param[out] data_length      On success, the number of bytes
 *                              that make up the key domain parameters data.
 *
 * \retval #PSA_SUCCESS
 * \retval #PSA_ERROR_BUFFER_TOO_SMALL
 */
psa_status_t psa_get_key_domain_parameters(
    const psa_key_attributes_t *attributes,
    uint8_t *data,
    size_t data_size,
    size_t *data_length);

/** Safe output buffer size for psa_get_key_domain_parameters().
 *
 * This macro returns a compile-time constant if its arguments are
 * compile-time constants.
 *
 * \warning This function may call its arguments multiple times or
 *          zero times, so you should not pass arguments that contain
 *          side effects.
 *
 * \note This is an experimental extension to the interface. It may change
 *       in future versions of the library.
 *
 * \param key_type  A supported key type.
 * \param key_bits  The size of the key in bits.
 *
 * \return If the parameters are valid and supported, return
 *         a buffer size in bytes that guarantees that
 *         psa_get_key_domain_parameters() will not fail with
 *         #PSA_ERROR_BUFFER_TOO_SMALL.
 *         If the parameters are a valid combination that is not supported
 *         by the implementation, this macro shall return either a
 *         sensible size or 0.
 *         If the parameters are not valid, the
 *         return value is unspecified.
 */
#define PSA_KEY_DOMAIN_PARAMETERS_SIZE(key_type, key_bits)              \
    (PSA_KEY_TYPE_IS_RSA(key_type) ? sizeof(int) :                      \
     PSA_KEY_TYPE_IS_DH(key_type) ? PSA_DH_KEY_DOMAIN_PARAMETERS_SIZE(key_bits) : \
     PSA_KEY_TYPE_IS_DSA(key_type) ? PSA_DSA_KEY_DOMAIN_PARAMETERS_SIZE(key_bits) : \
     0)
#define PSA_DH_KEY_DOMAIN_PARAMETERS_SIZE(key_bits)     \
    (4 + (PSA_BITS_TO_BYTES(key_bits) + 5) * 3 /*without optional parts*/)
#define PSA_DSA_KEY_DOMAIN_PARAMETERS_SIZE(key_bits)    \
    (4 + (PSA_BITS_TO_BYTES(key_bits) + 5) * 2 /*p, g*/ + 34 /*q*/)

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_EXTRA_H */
