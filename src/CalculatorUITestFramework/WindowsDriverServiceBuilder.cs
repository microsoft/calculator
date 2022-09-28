//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//See the NOTICE file distributed with this work for additional
//information regarding copyright ownership.
//You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

//Portions Copyright(c) Microsoft Corporation

using System;
using System.IO;
using System.Net;
using System.Net.Sockets;

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
            if (this.FileInfo == null)
            {
                this.FileInfo = new FileInfo(@"c:\Program Files (x86)\Windows Application Driver\winappdriver.exe");
            }
            return new WindowsDriverLocalService(this.FileInfo, string.Empty, IPAddress.Parse(this.IpAddress), this.Port, this.StartUpTimeout);
        }

        public WindowsDriverServiceBuilder WithFileInfo(FileInfo fileInfo)
        {
            this.FileInfo = fileInfo ?? throw new ArgumentNullException("FileInfo should not be NULL");
            return this;
        }

        public WindowsDriverServiceBuilder WithStartUpTimeOut(TimeSpan startUpTimeout)
        {
            this.StartUpTimeout = startUpTimeout;
            return this;
        }

        public WindowsDriverServiceBuilder WithIPAddress(string ipAddress)
        {
            this.IpAddress = ipAddress;
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

            this.Port = port;
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
                this.Port = ((IPEndPoint)sock.LocalEndPoint).Port;
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
