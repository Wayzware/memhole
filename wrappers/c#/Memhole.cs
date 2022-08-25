using System.Diagnostics;

/*
 * Memhole C# Wrapper v1.0 for Memhole v1.2.x
 * This library allows for easy use of the Memhole kernel module for Linux
 * 
 * Note: unlike the v1.6+ C/C++ wrapper, this wrapper is not thread safe
 * You must implement thread safety on your own if you plan to access memhole
 * from multiple threads. (The memhole device itself is always locked to 1 instance)
 */

namespace Memhole
{
    public enum ErrorCodes : int
    {
        InvalidDevice = 4,
        MemholeDeviceNotFound = 8,
        MemholeDeviceBusy = 16,
        InvalidPID = 32,
        MemholeDeviceKmallocFail = 64
    };

    public class Memhole
    {
        private const string MEMHOLE_DEVICE_PATH = "/dev/memhole";

        private const int SEEK_MODE_SET_PID = 0;
        private const int SEEK_MODE_SET_POS = 1;
        private const int SEEK_MODE_GET_POS = 2;
        private const int SEEK_MODE_SET_BUF = 3;

        long bufferSize = 0;
        bool canWrite = false;
        FileStream? device = null;
        
        public Memhole(bool write = false)
        {
            canWrite = write;
        }

        public int Connect()
        {
            if(device != null)
            {
                return -((int) ErrorCodes.InvalidDevice);
            }

            if (!File.Exists(MEMHOLE_DEVICE_PATH))
            {
                return -((int)ErrorCodes.MemholeDeviceNotFound);
            }

            try
            {
                if (canWrite)
                {
                    device = File.Open(MEMHOLE_DEVICE_PATH, FileMode.Open, FileAccess.ReadWrite);
                }
                else
                {
                    device = File.Open(MEMHOLE_DEVICE_PATH, FileMode.Open, FileAccess.Read);
                }
            }
            catch
            {
                return -((int)ErrorCodes.MemholeDeviceBusy);
            }

            bufferSize = 0;
            return 0;
        }

        public int Disconnect()
        {
            if (device == null)
            {
                return -((int)ErrorCodes.InvalidDevice);
            }

            try
            {
                device.Close();
            }
            catch
            {
                return -((int)ErrorCodes.InvalidDevice);
            }

            device = null;
            return 0;
        }

        public int AttachToPid(int pid)
        {
            if (device == null)
            {
                return -((int)ErrorCodes.InvalidDevice);
            }
            if(!Process.GetProcesses().Any(x => x.Id == pid))
            {
                return -((int) ErrorCodes.InvalidPID);
            }

            long ret = device.Seek((long)pid, SEEK_MODE_SET_PID);
            return ret == 0 ? -((int) ErrorCodes.InvalidPID) : (int) ret;
        }

        public int SetMemoryPosition(long pos)
        {
            if (device == null)
            {
                return -((int)ErrorCodes.InvalidDevice);
            }

            return (int) device.Seek((long)pos, (SeekOrigin)(SEEK_MODE_SET_POS));
        }

        public int GetMemoryPosition()
        {
            if (device == null)
            {
                return -((int)ErrorCodes.InvalidDevice);
            }

            return (int)device.Seek((long)0, (SeekOrigin)(SEEK_MODE_GET_POS));
        }

        public int SetBufferSize(long len)
        {
            if (device == null)
            {
                return -((int)ErrorCodes.InvalidDevice);
            }

            if(len < bufferSize)
            {
                if(device.Seek(len, (SeekOrigin)(SEEK_MODE_SET_BUF)) != 0)
                {
                    return -((int)ErrorCodes.MemholeDeviceKmallocFail);
                }
            }

            return 0;
        }

        public byte[]? ReadMemory(int len)
        {
            if (device == null)
            {
                return null;
            }

            if(bufferSize < len)
            {
                if(SetBufferSize(len) != 0)
                {
                    return null;
                }
            }

            byte[] result = new byte[len];
            int bytesRead = device.Read(result, 0, len);
            if(bytesRead < result.Length)
            {
                Array.Resize(ref result, bytesRead);
            }
            return result;
        }

        public int WriteMemory(byte[] data)
        {
            if (device == null)
            {
                return -((int)ErrorCodes.InvalidDevice);
            }

            if (bufferSize < data.Count())
            {
                if (SetBufferSize(data.Count()) != 0)
                {
                    return -((int)ErrorCodes.MemholeDeviceKmallocFail);
                }
            }

            device.Write(data, 0, data.Count());

            return 0;
        }


    }
}
