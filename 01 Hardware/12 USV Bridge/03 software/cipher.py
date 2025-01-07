"""

Program history
25.09.2024    V. 01.00    init Version cipher modul (Author Franke)

cipher modul with
functions
  - generateKey - generate Key and save it to keyfile
  - encrypt - encrypted data bytes with a key, saved in the keyfile
  - decrypt - decrypted a encrypted string with a key, saved in the keyfile
"""

__version__ = '01.00'
__author__ = 'S. Franke'

def generateKey(keyFile: str):
    """
    Generate random Key and save it to key file.
    generateKey(keyFile: str)
    - input value
      keyFile - key file to store the generated key : str
    - output value
      None
    """
    import uos
    import binascii
    
    keyBase64 = binascii.b2a_base64(uos.urandom(32))
    print(keyBase64)
    fid = open(keyFile, 'wb')
    fid.write(keyBase64)
    fid.close()
    return

def encrypt(keyFile: str, data: bytes)->str:
    """
    Encrypt data array to an base64 encypted string, use the key from the key file.
     Save the padding value as the first byte in the data array and 
     expand the data array with number of padding bytes.
    encrypt(keyFile: str, data: bytes)
    - input value
      keyFile - key file to store the generated key : str
      data - to encrypted bytes array : bytes
    - output value
      enypted data string : str
    """
    from ucryptolib import aes
    import binascii
    MODE_ECB = 1
    MODE_CBC = 2
    MODE_CTR = 6
    BLOCK_SIZE = 16
    
    fid = open(keyFile, 'r')
    keyBase64 = fid.read()
    fid.close()
    
    cipher = aes(binascii.a2b_base64(keyBase64), MODE_ECB)
    
    pad = (BLOCK_SIZE - len(data) - 1) % BLOCK_SIZE
    nPad = int(pad).to_bytes(1,'little')
    
    dataExtend = nPad + data + b"\x00"*pad
    
    dataEncrypted = cipher.encrypt(dataExtend)
    encryptedStr = binascii.b2a_base64(dataEncrypted).decode()
    return encryptedStr

def decrypt(keyFile: str, encryptedStr: str)->bytes:
    """
    Decrypt base64 data string to an bytes array, use the key from the key file.
     Load the value of padding bytes from the first byte in the decrypted data array and 
     reduce the data array with this value.
    encrypt(keyFile: str, data: bytes)
    - input value
      keyFile - key file to store the generated key : str
      encryptedStr - to decrypted data string : str
    - output value
      decrypted data array : bytes
    """
    from ucryptolib import aes
    import binascii
    MODE_ECB = 1
    MODE_CBC = 2
    MODE_CTR = 6
    BLOCK_SIZE = 16
    
    fid = open(keyFile, 'r')
    keyBase64 = fid.read()
    fid.close()
    
    cipher = aes(binascii.a2b_base64(keyBase64), MODE_ECB)
    
    dataEncrypted = binascii.a2b_base64(encryptedStr)
    data = cipher.decrypt(dataEncrypted)
    pad = data[0] #1 byte is int, int.from_bytes(1,'little')
    
    return data[1:-pad]

def test():
    from ucryptolib import aes
    import binascii
    import json

    MODE_ECB = 1
    MODE_CBC = 2
    MODE_CTR = 6
    BLOCK_SIZE = 16
    key = b'I_am_32bytes=256bits_key_padding'

    cipher = aes(key, MODE_ECB)

    plaintext = 'This is AES cryptographic!'
    print('Plain Text:', plaintext)
    print(' ')
    base64 = binascii.b2a_base64(plaintext)
    print('base64 Text:', base64)

    pad = BLOCK_SIZE - len(base64) % BLOCK_SIZE
    base64Extend = base64 + " "*pad

    encrypted = cipher.encrypt(base64Extend)
    print('AES-ECB encrypted:', encrypted )
    cipher = aes(key,MODE_ECB) # cipher has to renew for decrypt
    decrypted64 = cipher.decrypt(encrypted)
    print('AES-ECB decrypted:', decrypted64)

    plaintext = binascii.a2b_base64(decrypted64).decode()
    print('AES-ECB decrypted:', plaintext)


    # as json
    cipher = aes(key, MODE_ECB)

    plaintext = 'This is AES cryptographic!'
    print('Plain Text:', plaintext)
    print(' ')
    jsonDat = json.dumps(plaintext)
    print('json Text:', jsonDat)

    pad = BLOCK_SIZE - len(jsonDat) % BLOCK_SIZE
    jsonDatExtend = jsonDat + " "*pad

    encrypted = cipher.encrypt(jsonDatExtend)
    print('AES-ECB encrypted:', encrypted )
    cipher = aes(key,MODE_ECB) # cipher has to renew for decrypt
    decryptedjsonDat = cipher.decrypt(encrypted)
    print('AES-ECB decrypted:', decryptedjsonDat)

    plaintext = json.loads(decryptedjsonDat)
    print('json decrypted:', plaintext)
