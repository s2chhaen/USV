"""
Created on 13.09.2024 17:01:00

Program history
13.09.2024    V. 01.00    init Version from Ringbuffer modul and RINGBUFFER class used the finish c-function as template(Author Franke)
25.10.2024    V. 01.10    new version with mutex lock for write and read data

Ringbuffer Modul with
RINGBUFFER Class
- Ringbuffer for asynchronous io

@author: S. Franke
"""

__version__ = '01.10'
__author__ = 'S. Franke'

class RINGBUFFER:
    """
    RINGBUFFER Class
      - Ringbuffer class for asynchronous data input and output
      - lock data read and write with an internal mutex
      - can used between two or more threads
    RINGBUFFER(size:int):
    - input value
      size - size for the buffer : integer bigger than 1
    - output value
      RINGBUFFER object
    """
    def __init__(self, size:int):
        self.size = max(size,2)+1
        self.data = bytearray(self.size)
        self.index_put = 0
        self.index_get = 0
        self.Mutex = __import__("_thread").allocate_lock()
        
    def put(self, value:int):
        """
        Put a byte to the RINGBUFFER
          Check for write one byte and check internal Mutex are free
        RINGBUFFER(size:int):
        - input value
          value : int 0..255
        - output value
          if function put has written the byte to buffer : Boolean
        """
        ret = False
        next_index = self.__modMax(self.index_put + 1)
        # check for overflow
        if self.index_get != next_index:
            self.data[self.index_put] = value
            self.Mutex.acquire()
            self.index_put = next_index
            self.Mutex.release()
            ret = True
        return ret
        
    def get(self)->int:
        """
        Get a byte from the RINGBUFFER
          Check for write one byte and check internal Mutex are free
        RINGBUFFER(size:int):
        - input value
          None
        - output value
          byte as Integer : int 0..255 |
          None when no byte are readable : None
        """
        ret = None
        if self.index_get != self.index_put:
            value = self.data[self.index_get]
            next_index = self.__modMax(self.index_get + 1)
            self.Mutex.acquire()
            self.index_get = next_index
            self.Mutex.release()
            ret = value
        return ret
    
    def copyGet(self,length:int)->bytearray:
        """
        Get a bytearray with specified length from the RINGBUFFER
          Check befor read the length and check internal Mutex are free
        RINGBUFFER(size:int):
        - input value
          length - specified read length : int > 0
        - output value
          bytearry with data from the buffer, length are maximal readable length from ringbuffer : bytearry object
          None when no bytes are readable : None
        """
        max_read_length = self.diff_readdata()
        if length >= 0:
            copy_length = min(length,max_read_length)
        else:
            copy_length = max_read_length
        read_length_block = min((copy_length),(self.size - self.index_get));
        data = None
        if read_length_block != 0:
            data = self.data[self.index_get:(self.index_get+read_length_block)]
            read_length_block = copy_length - read_length_block;
            if read_length_block != 0:
                data = data+self.data[0:read_length_block]
            next_index = self.__modMax(self.index_get + copy_length);
            self.Mutex.acquire()
            self.index_get = next_index
            self.Mutex.release()
        return data
    
    def copyPut(self,data:bytearray)->int:
        """
        Put a bytearray with specified length to the RINGBUFFER
          Check befor copy the length and the internal Mutex are free
        copyPut(data:bytearray):
        - input value
          data - data in a bytearray object to write in the ringbuffer : bytearray > 0
        - output value
          copy_length - integer with maximal copied data into the ringbuffer : int
        """
        length = len(data)
        # berechne die maximale Schreiblänge
        write_length = self.diff_writedata()
        if length > write_length:
            print("-----RB can not copy all data-----")
        copy_length = min(length, write_length)
        # Berechne die Länge für die erste Kopieranweisungen
        write_length_block = min(copy_length,(self.size - self.index_put))
        # schreibe in den Ringpuffer
        if write_length_block != 0:
            # kopiere ersten Datenblock
            self.data[self.index_put:self.index_put+write_length_block] = data[0:write_length_block]
            p_write_start = write_length_block
            # Berechne die Länge für die zweite Kopieranweisungen
            write_length_block = copy_length - write_length_block
            if write_length_block != 0:
                 # kopiere zeiten Datenblock
                self.data[0:write_length_block] = data[p_write_start:p_write_start+write_length_block]
            next_index = self.__modMax(self.index_put+copy_length)
            self.Mutex.acquire()
            self.index_put = next_index
            self.Mutex.release()
        return copy_length
    
    def flush(self):
        """
        Flush the internal RINGBUFFER
        flush():
        - input value
          None
        - output value
          None
        """
        self.index_put = 0
        self.index_get = 0
        return
    
    def isEmpty(self)->bool:
        """
        Check if internal RINGBUFFER is empty
        isEmpty():
        - input value
          None
        - output value
          free - Boolean for the result : bool
        """
        return self.index_get == self.index_put
    
    def diff_readdata(self)->int:
        """
        Get number for readable data
        diff_readdata():
        - input value
          None
        - output value
          Number of readable data : int
        """
        return self.__modMax((self.index_put + self.size) - self.index_get)
    
    def diff_writedata(self)->int:
        """
        Get number of writeable length
        diff_writedata():
        - input value
          None
        - output value
          Number of writeable length : int
        """
        return self.__modMax((self.index_get + (self.size-1)) - self.index_put)
    
    def chk_for_writedata(self)->bool:
        """
        Check Ringbuffer for write data
        chk_for_writedata():
        - input value
          None
        - output value
          ifWritedata - Boolean for the result : bool
        """
        return (self.index_get != self.__modMax(self.index_put + 1))
    
    def chk_for_readdata(self)->bool:
        """
        Check Ringbuffer for readable data
        chk_for_readdata():
        - input value
          None
        - output value
          ifReaddata - Boolean for the result : bool
        """
        return self.index_get != self.index_put
    
    def __modMax(self,val)->int:
        """
        private function:
        Modulo value with the maximal Ringbuffer length.
        __modMax():
        - input value
          val - value to modulo with the max length of the Ringbuffer
        - output value
          modMax - modulo value : int
        """
        return val % self.size
    
    def __del__(self):
        """
        Deinitialisation the object and delete the internal values. Used in Python.
        __del__():
        - input value
          None
        - output value
          None
        """
        del self.size
        del self.index_put
        del self.index_get
        del self.data
        del self
        return
    
    def deinit(self):
        """
        Deinitialisation the object and delete the internal values. Used in Micropython.
        deinit():
        - input value
          None
        - output value
          None
        """
        self.__del__()
        return
    
if __name__ == '__main__':
    def Test():
        RB = RINGBUFFER(8)
        RB.put(1)
        RB.put(2)
        RB.put(3)
        RB.put(4)
        RB.put(5)
        print(RB.diff_readdata())
        print(RB.copyGet(4))
        print(RB.diff_readdata())
        RB.put(6)
        RB.put(7)
        RB.put(8)
        RB.put(9)
        RB.put(10)
        RB.put(11)
        print(RB.data)
        print(RB.diff_readdata())
        print(RB.copyGet(6))
        print(RB.diff_readdata())
        RB.put(11)
        RB.put(12)
        RB.put(13)
        print(RB.copyGet(-1))
        print(RB.diff_readdata())
        print(RB.copyPut(bytearray(b'0123456789')))
        print(RB.data)
        print(RB.diff_readdata())
        print(RB.copyGet(-1))
        return RB
    Test()