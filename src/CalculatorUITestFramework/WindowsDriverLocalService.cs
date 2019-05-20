using OpenQA.Selenium.Appium.Service;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Runtime.CompilerServices;
using System.Text;

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
            FileName = fileName;
            Arguments = arguments;
            IP = ip;
            Port = port;
            InitializationTimeout = initializationTimeout;
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        public void Start()
        {
            if (IsRunning)
            {
                return;
            }

            Service = new Process();
            Service.StartInfo.FileName = FileName.FullName;
            Service.StartInfo.Arguments = Arguments;
            Service.StartInfo.UseShellExecute = false;
            Service.StartInfo.CreateNoWindow = true;

            Service.StartInfo.RedirectStandardOutput = true;
            Service.OutputDataReceived += (sender, e) => OutputDataReceived?.Invoke(this, e);

            bool isLaunched = false;
            string msgTxt =
                $"The local WinAppDriver server has not been started: {FileName.FullName} Arguments: {Arguments}. " +
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
                if (Service == null)
                {
                    return false;
                }

                try
                {
                    var pid = Service.Id;
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
            get { return new Uri($"http://{IP.ToString()}:{Convert.ToString(Port)}"); }
        }

        private void DestroyProcess()
        {
            if (Service == null)
            {
                return;
            }

            try
            {
                Service.Kill();
            }
            catch (Exception)
            {
            }
            finally
            {
                Service.Close();
            }
        }

        private bool Ping()
        {
            bool pinged = false;

            Uri status;

            Uri service = ServiceUrl;
            if (service.IsLoopback)
            {
                status = new Uri("http://localhost:" + Convert.ToString(Port) + "/status");
            }
            else
            {
                status = new Uri(service.ToString() + "/status");
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
