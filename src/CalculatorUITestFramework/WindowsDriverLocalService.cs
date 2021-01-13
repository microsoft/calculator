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
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Runtime.CompilerServices;

namespace CalculatorUITestFramework
{
    public class WindowsDriverLocalService : IDisposable
    {
        private FileInfo FileName;
        private string Arguments;
        private IPAddress IP;
        private int Port;
        private TimeSpan InitializationTimeout;
        private Process Service;

        public event DataReceivedEventHandler OutputDataReceived;

        internal WindowsDriverLocalService(
            FileInfo fileName,
            string arguments,
            IPAddress ip,
            int port,
            TimeSpan initializationTimeout)
        {
            this.FileName = fileName;
            this.Arguments = arguments;
            this.IP = ip;
            this.Port = port;
            this.InitializationTimeout = initializationTimeout;
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        public void Start()
        {
            if (this.IsRunning)
            {
                return;
            }

            this.Service = new Process();
            this.Service.StartInfo.FileName = FileName.FullName;
            this.Service.StartInfo.Arguments = Arguments;
            this.Service.StartInfo.UseShellExecute = false;
            this.Service.StartInfo.CreateNoWindow = true;

            this.Service.StartInfo.RedirectStandardOutput = true;
            this.Service.OutputDataReceived += (sender, e) => OutputDataReceived?.Invoke(this, e);

            bool isLaunched = false;
            string msgTxt =
                $"The local WinAppDriver server has not been started: {this.FileName.FullName} Arguments: {this.Arguments}. " +
                "\n";

            try
            {
                Service.Start();

                Service.BeginOutputReadLine();
            }
            catch (Exception e)
            {
                DestroyProcess();
                throw new Exception(msgTxt, e);
            }

            isLaunched = Ping();
            if (!isLaunched)
            {
                DestroyProcess();
                throw new Exception(
                    msgTxt +
                    $"Time {InitializationTimeout.TotalMilliseconds} ms for the service starting has been expired!");
            }
        }

        public bool IsRunning
        {
            get
            {
                if (this.Service == null)
                {
                    return false;
                }

                try
                {
                    var pid = this.Service.Id;
                }
                catch (Exception)
                {
                    return false;
                }

                return Ping();
            }
        }

        public void Dispose()
        {
            DestroyProcess();
            GC.SuppressFinalize(this);
        }

        public Uri ServiceUrl
        {
            // Note: append /wd/hub to the URL if you're directing the test at Appium
            get { return new Uri($"http://{this.IP}:{Convert.ToString(this.Port)}"); }
        }

        private void DestroyProcess()
        {
            if (this.Service == null)
            {
                return;
            }

            try
            {
                this.Service.Kill();
            }
            catch (Exception)
            {
            }
            finally
            {
                this.Service.Close();
            }
        }

        private bool Ping()
        {
            bool pinged = false;

            Uri status;

            Uri service = this.ServiceUrl;
            if (service.IsLoopback)
            {
                status = new Uri("http://localhost:" + Convert.ToString(this.Port) + "/status");
            }
            else
            {
                status = new Uri(service + "/status");
            }

            DateTime endTime = DateTime.Now.Add(this.InitializationTimeout);
            while (!pinged & DateTime.Now < endTime)
            {
                HttpWebRequest request = (HttpWebRequest)HttpWebRequest.Create(status);
                HttpWebResponse response = null;
                try
                {
                    using (response = (HttpWebResponse)request.GetResponse())
                    {
                        pinged = true;
                    }
                }
                catch (Exception)
                {
                    pinged = false;
                }
                finally
                {
                    if (response != null)
                    {
                        response.Close();
                    }
                }
            }
            return pinged;
        }
    }
}
