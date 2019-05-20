using OpenQA.Selenium;
using OpenQA.Selenium.Appium.Service;
using OpenQA.Selenium.Appium.Service.Options;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace CalculatorUITestFramework
{
    public class WindowsDriverServiceBuilder
    {
        private string IpAddress = "127.0.0.1";
        private int Port = 4723;
        private TimeSpan StartUpTimeout = new TimeSpan(0, 2, 0);
        private FileInfo FileInfo;

        public WindowsDriverLocalService Build()
        {
            if (FileInfo == null)
            {
                FileInfo = new FileInfo(@"c:\Program Files (x86)\Windows Application Driver\winappdriver.exe");
            }
            return new WindowsDriverLocalService(FileInfo, string.Empty, IPAddress.Parse(this.IpAddress), this.Port, StartUpTimeout);
        }

        public WindowsDriverServiceBuilder WithFileInfo(FileInfo fileInfo)
        {
            if (fileInfo == null)
            {
                throw new ArgumentNullException("FileInfo should not be NULL");
            }
            FileInfo = fileInfo;
            return this;
        }

        public WindowsDriverServiceBuilder WithStartUpTimeOut(TimeSpan startUpTimeout)
        {
            if (startUpTimeout == null)
            {
                throw new ArgumentNullException("A startup timeout should not be NULL");
            }
            StartUpTimeout = startUpTimeout;
            return this;
        }

        public WindowsDriverServiceBuilder WithIPAddress(string ipAddress)
        {
            IpAddress = ipAddress;
            return this;
        }

        public WindowsDriverServiceBuilder UsingPort(int port)
        {
            if (port < 0)
            {
                throw new ArgumentException("The port parameter should not be negative");
            }

            if (port == 0)
            {
                return UsingAnyFreePort();
            }

            Port = port;
            return this;
        }

        public WindowsDriverServiceBuilder UsingAnyFreePort()
        {
            Socket sock = null;

            try
            {
                sock = new Socket(AddressFamily.InterNetwork,
                    SocketType.Stream, ProtocolType.Tcp);
                sock.Bind(new IPEndPoint(IPAddress.Any, 0));
                Port = ((IPEndPoint)sock.LocalEndPoint).Port;
                return this;
            }
            finally
            {
                if (sock != null)
                {
                    sock.Dispose();
                }
            }
        }
    }
}
