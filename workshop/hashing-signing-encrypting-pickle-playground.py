# hashing-signing-encrypting-pickle-playground.py
# Version 20160503
# Written by Harry Wong (harryw1)

### Libraries ###
import pickle
from Crypto.Cipher import AES
from Crypto.Hash import SHA256
from Crypto.PublicKey import RSA
from Crypto.Random import OSRNG

### Constants ###
RSA_SIZE = 1024
RANDOM_STR_LEN = 8
MESSAGE = "top secret message - the cake is a lie"
PICKLE_FILENAME = "pickle-file.p"

### Functions ###
def generate_private_public_rsa_keys():
    """Generate and return private and public RSA keys."""
    # Generate random number generator.
    rng = OSRNG.new().read

    private_rsa_key = RSA.generate(RSA_SIZE, rng)  # Generate private RSA key.
    public_rsa_key = private_rsa_key.publickey()   # Generate public RSA key.

    return private_rsa_key, public_rsa_key

def get_hash_value(s):
    """Get the SHA256 hash value of the string."""
    return SHA256.new(s).digest()

def get_signature(s, private_rsa_key):
    """Get the signature of the string using the private RSA key."""
    return private_rsa_key.sign(s, "")

def verify_signature(s, rsa_key, signature):
    """Verify the signature of the string using the private or public RSA
    key."""
    return rsa_key.verify(s, signature)

def get_encrypted(s, rsa_key):
    """Get the encrypted string of the string using the private or public RSA
    key."""
    # Generate random string.
    random_str = OSRNG.new().read(RANDOM_STR_LEN)

    return rsa_key.encrypt(s, random_str)

def get_decrypted(encrypted_str, private_rsa_key):
    """Get the decrypted string of the encrypted string using the private RSA
    key."""
    return private_rsa_key.decrypt(encrypted_str)

### Main ###
private_rsa_key, public_rsa_key = generate_private_public_rsa_keys()

# Hashing.
print "Getting SHA256 hash value of message \"{0}\"".format(MESSAGE)
hash_value = get_hash_value(MESSAGE)
print "  The SHA256 hash value is \"{0}\"".format(repr(hash_value))
print "\n"

# Signing.
try:
    print "Getting signature of message hash value \"{0}\" with public RSA " \
        "key...".format(repr(hash_value))
    signature = get_signature(hash_value, public_rsa_key)
    assert(False)
except TypeError:
    print "  Cannot get signature with public RSA key"
except AssertionError:
    print "  ERROR: Got signature with public RSA key"
print "Getting signature of message hash value \"{0}\" with private RSA " \
    "key...".format(repr(hash_value))
signature = get_signature(hash_value, private_rsa_key)
print "  The signature is \"{0}\"".format(signature)
assert(private_rsa_key.verify(hash_value, signature) == True)
assert(public_rsa_key.verify(hash_value, signature) == True)
assert(private_rsa_key.verify(hash_value[:-1], signature) == False)
assert(public_rsa_key.verify(hash_value[:-1], signature) == False)
print "\n"

# Encrypting and decrypting.
print "Getting encrypted message of message \"{0}\" with (public) RSA " \
    "key".format(MESSAGE)
encrypted_message = get_encrypted(MESSAGE, public_rsa_key)
assert(encrypted_message == get_encrypted(MESSAGE, private_rsa_key))
print "  The encrypted message is \"{0}\"".format(encrypted_message)
try:
    print "Getting decrypted message of encrypted message with public RSA key"
    decrypted_message = get_decrypted(encrypted_message, public_rsa_key)
    assert(False)
except TypeError:
    print "  Cannot get decrypted message with public RSA key"
except AssertionError:
    print "  ERROR: Got decrypted message with public RSA key"
print "Getting decrypted message of encrypted message with private RSA key"
decrypted_message = get_decrypted(encrypted_message, private_rsa_key)
print "  The decrypted message is \"{0}\"".format(decrypted_message)
print "\n"

# Dumping and loading public RSA key.
print 'Dumping public RSA key to pickle file...'
pickle_file = open(PICKLE_FILENAME, 'w')
pickle.dump(public_rsa_key, pickle_file)
pickle_file.close()
print 'Loading public RSA key from pickle file...'
pickle_file = open(PICKLE_FILENAME, 'r')
public_rsa_key = pickle.load(pickle_file)
pickle_file.close()
