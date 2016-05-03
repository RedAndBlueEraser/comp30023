# sending-encrypted-message.py
# Version 20160503
# Written by Harry Wong (harryw1)

### Libraries ###
import pickle
from Crypto.Cipher import AES
from Crypto.Cipher import PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Random import OSRNG

### Constants ###
RSA_SIZE = 1024
PUBLIC_RSA_KEY_FILENAME = "PublicRSAKey"
AES_KEY_SIZE_IN_BYTES = 16
MESSAGE = "tldr; encryption"
ENCRYPTED_AES_KEY_FILENAME = "EncryptedAESKey"
ENCRYPTED_MESSAGE_FILENAME = "EncryptedMessage"

### Functions ###
def receiver():
    # Generate random number generator.
    rng = OSRNG.new().read

    # Generate private and public RSA keys.
    private_rsa_key = RSA.generate(RSA_SIZE, rng)
    public_rsa_key = private_rsa_key.publickey()

    # Dump and share public RSA key.
    public_rsa_key_file = open(PUBLIC_RSA_KEY_FILENAME, "w")
    #pickle.dump(public_rsa_key, public_rsa_key_file)
    public_rsa_key_file.write(public_rsa_key.exportKey())
    public_rsa_key_file.close()

    # Wait for sender to send encrypted AES key and encrypted message.
    sender()

    # Load encrypted AES key.
    encrypted_aes_key_file = open(ENCRYPTED_AES_KEY_FILENAME, 'r')
    encrypted_aes_key = pickle.load(encrypted_aes_key_file)
    encrypted_aes_key_file.close()

    # Load encrypted message.
    encrypted_message_file = open(ENCRYPTED_MESSAGE_FILENAME, 'r')
    encrypted_message = pickle.load(encrypted_message_file)
    iv = pickle.load(encrypted_message_file)
    encrypted_message_file.close()

    # Decrypt AES key with private RSA key.
    rsa_oaep_cipher = PKCS1_OAEP.new(private_rsa_key)
    aes_key = rsa_oaep_cipher.decrypt(encrypted_aes_key)

    # Decrypt message with AES key.
    aes_cipher = AES.new(aes_key, AES.MODE_CBC, iv)
    decrypted_message = aes_cipher.decrypt(encrypted_message)

    # Read decrypted_message.
    print decrypted_message

    return

def sender():
    # Load public RSA key.
    public_rsa_key_file = open(PUBLIC_RSA_KEY_FILENAME, "r")
    #public_rsa_key = pickle.load(public_rsa_key_file)
    public_rsa_key = RSA.importKey(public_rsa_key_file.read())
    public_rsa_key_file.close()

    # Generate random number generator.
    rng = OSRNG.new().read

    # Generate AES key.
    aes_key = rng(AES_KEY_SIZE_IN_BYTES)

    # Encrypt AES key with public RSA key.
    rsa_oaep_cipher = PKCS1_OAEP.new(public_rsa_key)
    encrypted_aes_key = rsa_ciphertext = rsa_oaep_cipher.encrypt(aes_key)

    # Dump encrypted AES key.
    encrypted_aes_key_file = open(ENCRYPTED_AES_KEY_FILENAME, "w")
    pickle.dump(encrypted_aes_key, encrypted_aes_key_file)
    encrypted_aes_key_file.close()

    # Encrypt message with AES key.
    iv = rng(AES_KEY_SIZE_IN_BYTES)
    aes_cipher = AES.new(aes_key, AES.MODE_CBC, iv)
    encrypted_message = aes_cipher.encrypt(MESSAGE)

    # Dump encrypted message.
    encrypted_message_file = open(ENCRYPTED_MESSAGE_FILENAME, "w")
    pickle.dump(encrypted_message, encrypted_message_file)
    pickle.dump(iv, encrypted_message_file)
    encrypted_message_file.close()

    return

### Main ###
receiver()  # Start receiver.
