using System;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;

namespace SharpSync
{
    public partial class Form1 : Form
	{
        int allThreads = 0;

		Process ChildProcess = null;
		EventWaitHandle evStart = new EventWaitHandle(false, EventResetMode.AutoReset, "EventStart");
		EventWaitHandle evStop = new EventWaitHandle(false, EventResetMode.AutoReset, "EventStop");
		EventWaitHandle evQuit = new EventWaitHandle(false, EventResetMode.AutoReset, "EventQuit");
        EventWaitHandle evStartThread = new EventWaitHandle(false, EventResetMode.AutoReset, "EventStartThread");
        EventWaitHandle evConfirm = new EventWaitHandle(false, EventResetMode.AutoReset, "EventConfirm");
		public Form1()
		{
			InitializeComponent();
		}

		private void button1_Click(object sender, EventArgs e) // START BUTTON
		{
			if (ChildProcess == null || ChildProcess.HasExited)
			{
				ChildProcess = Process.Start("MFCApplication.exe");
                allThreads = 0;

                threadsBox.Enabled = true;
                button2.Enabled = true;
            }
            else if (threadsBox.Text!=null && threadsBox.Text!="")
            {
                int threadsNum = 0;
                Int32.TryParse(threadsBox.Text, out threadsNum);

                for(int i = 0; i < threadsNum; i++)
                {
                    evStartThread.Set();
                    
                    evConfirm.WaitOne();
                    if (allThreads == 0) listBox1.Items.Add("Started main thread.");
                    listBox1.Items.Add("Started thread number: " + allThreads);
                    allThreads++;
                }
            }
		}

		private void button2_Click(object sender, EventArgs e) // STOP BUTTON
		{
			if (ChildProcess != null && !ChildProcess.HasExited)
			{
                if (allThreads > 0)
                {
                    evStop.Set();

                    evConfirm.WaitOne();
                    listBox1.Items.Add("Got the confirm that thread " + (allThreads-1) + " stopped!");
                    allThreads--;

                    if (allThreads == 0) listBox1.Items.Add("Stopped main thread.");
                }
                else
                {
                    listBox1.Items.Add("There are 0 threads active! Start some.");
                }                
            }
            else {
                listBox1.Items.Add("Start the c++ .exe first!");
                button2.Enabled = false;
                threadsBox.Enabled = false;
            }
		}

		private void Form1_FormClosed(object sender, FormClosedEventArgs e)
		{
            if (ChildProcess != null && !ChildProcess.HasExited)
            {
                listBox1.Items.Add("Waiting for handles to close...");
                evQuit.Set();
                evConfirm.WaitOne();
            }            
        }
	}
}
