using System;
using System.Drawing;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
 
namespace SixtyFourBox.UI
{
    internal static class Native64Box
    {
        // Name of the backend DLL (build your C backend as 64box_backend.dll).
        private const string DllName = "64box_backend";
 
        public enum CpuType : int
        {
            CPU_8086 = 0,
            CPU_8088 = 1,
            CPU_386  = 2,
            CPU_486  = 3
        }
 
        [StructLayout(LayoutKind.Sequential)]
        public struct CpuState
        {
            public ushort AX;
            public ushort BX;
            public ushort CX;
            public ushort DX;
 
            public ushort SP;
            public ushort BP;
            public ushort SI;
            public ushort DI;
 
            public ushort IP;
 
            public ushort CS;
            public ushort DS;
            public ushort ES;
            public ushort SS;
 
            public ushort FLAGS;
        }
 
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr s64box_create(CpuType cpuType);
 
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void s64box_destroy(IntPtr handle);
 
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool s64box_reset(IntPtr handle);
 
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool s64box_run_cycles(IntPtr handle, uint cycles);
 
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool s64box_load_rom(
            IntPtr handle,
            byte[] data,
            uint size,
            uint address);
 
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool s64box_get_cpu_state(
            IntPtr handle,
            out CpuState state);
 
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool s64box_get_text_video(
            IntPtr handle,
            [Out] byte[] chars_out,
            [Out] byte[]? attrs_out,
            uint cols,
            uint rows);
    }
 
    public class MainForm : Form
    {
        private ComboBox _cpuTypeCombo;
        private Button _loadRomButton;
        private Button _startButton;
        private Button _stopButton;
        private Button _resetButton;
        private TextBox _videoTextBox;
        private TextBox _logTextBox;
 
        // Backend-related state
        private IntPtr _emuHandle = IntPtr.Zero;
        private System.Windows.Forms.Timer _runTimer;
        private string? _loadedRomPath;
 
        public MainForm()
        {
            InitializeComponent();
            InitializeBackend();
        }
 
        private void InitializeBackend()
        {
            _runTimer = new System.Windows.Forms.Timer
            {
                Interval = 50 // ms
            };
            _runTimer.Tick += OnRunTimerTick;
 
            CreateEmulator();
        }
 
        private void InitializeComponent()
        {
            Text = "64Box v0.1";
            StartPosition = FormStartPosition.CenterScreen;
            Size = new Size(900, 600);
 
            // CPU type selection
            var cpuTypeLabel = new Label
            {
                Text = "CPU Type:",
                Location = new Point(10, 15),
                AutoSize = true
            };
 
            _cpuTypeCombo = new ComboBox
            {
                Location = new Point(80, 10),
                DropDownStyle = ComboBoxStyle.DropDownList,
                Width = 120
            };
            _cpuTypeCombo.Items.AddRange(new object[]
            {
                "8086",
                "8088",
                "386 (future)",
                "486 (future)"
            });
            _cpuTypeCombo.SelectedIndex = 0;
            _cpuTypeCombo.SelectedIndexChanged += OnCpuTypeChanged;
 
            // ROM load button
            _loadRomButton = new Button
            {
                Text = "Load ROM...",
                Location = new Point(220, 8),
                Width = 100
            };
            _loadRomButton.Click += OnLoadRomClicked;
 
            // Control buttons
            _startButton = new Button
            {
                Text = "Start",
                Location = new Point(340, 8),
                Width = 80
            };
            _startButton.Click += OnStartClicked;
 
            _stopButton = new Button
            {
                Text = "Stop",
                Location = new Point(430, 8),
                Width = 80,
                Enabled = false
            };
            _stopButton.Click += OnStopClicked;
 
            _resetButton = new Button
            {
                Text = "Reset",
                Location = new Point(520, 8),
                Width = 80
            };
            _resetButton.Click += OnResetClicked;
 
            // Video text display (80x25 text mode)
            _videoTextBox = new TextBox
            {
                Multiline = true,
                ScrollBars = ScrollBars.None,
                ReadOnly = true,
                Location = new Point(10, 50),
                Size = new Size(ClientSize.Width - 20, 420),
                Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right,
                Font = new Font(FontFamily.GenericMonospace, 9f)
            };
 
            // Log/output area
            _logTextBox = new TextBox
            {
                Multiline = true,
                ScrollBars = ScrollBars.Vertical,
                ReadOnly = true,
                Location = new Point(10, 480),
                Size = new Size(ClientSize.Width - 20, ClientSize.Height - 490),
                Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right,
                Font = new Font(FontFamily.GenericMonospace, 9f)
            };
 
            Controls.Add(cpuTypeLabel);
            Controls.Add(_cpuTypeCombo);
            Controls.Add(_loadRomButton);
            Controls.Add(_startButton);
            Controls.Add(_stopButton);
            Controls.Add(_resetButton);
            Controls.Add(_videoTextBox);
            Controls.Add(_logTextBox);
        }
 
        private void CreateEmulator()
        {
            DestroyEmulator();
 
            var cpuType = GetSelectedCpuType();
 
            try
            {
                _emuHandle = Native64Box.s64box_create(cpuType);
            }
            catch (DllNotFoundException ex)
            {
                AppendLog($"Backend DLL not found: {ex.Message}");
                AppendLog("UI will run without emulator backend. Place 64box_backend.dll next to the UI executable to enable emulation.");
 
                _emuHandle = IntPtr.Zero;
 
                // Disable backend-dependent controls so the user can't start a non-existent emulator.
                _startButton.Enabled = false;
                _stopButton.Enabled = false;
                _resetButton.Enabled = false;
                _loadRomButton.Enabled = false;
 
                return;
            }
 
            if (_emuHandle == IntPtr.Zero)
            {
                AppendLog("Failed to create emulator instance.");
            }
            else
            {
                AppendLog($"Emulator created with CPU type {cpuType}.");
            }
        }
 
        private void DestroyEmulator()
        {
            if (_emuHandle != IntPtr.Zero)
            {
                Native64Box.s64box_destroy(_emuHandle);
                _emuHandle = IntPtr.Zero;
                AppendLog("Emulator destroyed.");
            }
        }
 
        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            _runTimer?.Stop();
            DestroyEmulator();
            base.OnFormClosed(e);
        }
 
        private Native64Box.CpuType GetSelectedCpuType()
        {
            return _cpuTypeCombo.SelectedIndex switch
            {
                0 => Native64Box.CpuType.CPU_8086,
                1 => Native64Box.CpuType.CPU_8088,
                2 => Native64Box.CpuType.CPU_386,
                3 => Native64Box.CpuType.CPU_486,
                _ => Native64Box.CpuType.CPU_8086
            };
        }
 
        private void OnCpuTypeChanged(object? sender, EventArgs e)
        {
            AppendLog($"CPU type changed to {_cpuTypeCombo.SelectedItem}, recreating emulator.");
            CreateEmulator();
        }
 
        private void OnLoadRomClicked(object? sender, EventArgs e)
        {
            if (_emuHandle == IntPtr.Zero)
            {
                AppendLog("Emulator is not created; cannot load ROM.");
                return;
            }
 
            using var ofd = new OpenFileDialog
            {
                Title = "Select ROM / Binary",
                Filter = "Binary files (*.bin;*.rom)|*.bin;*.rom|All files (*.*)|*.*"
            };
 
            if (ofd.ShowDialog(this) == DialogResult.OK)
            {
                try
                {
                    var data = File.ReadAllBytes(ofd.FileName);
                    _loadedRomPath = ofd.FileName;
 
                    bool ok = Native64Box.s64box_load_rom(
                        _emuHandle,
                        data,
                        (uint)data.Length,
                        0x0000);
 
                    if (!ok)
                    {
                        AppendLog($"Failed to load ROM '{ofd.FileName}' into emulator.");
                    }
                    else
                    {
                        AppendLog($"ROM loaded: {ofd.FileName} ({data.Length} bytes at 0x0000).");
                    }
                }
                catch (Exception ex)
                {
                    AppendLog($"Error loading ROM: {ex.Message}");
                }
            }
        }
 
        private void OnStartClicked(object? sender, EventArgs e)
        {
            if (_emuHandle == IntPtr.Zero)
            {
                AppendLog("Cannot start: emulator handle is null.");
                return;
            }
 
            if (_loadedRomPath == null)
            {
                AppendLog("No ROM loaded. Load a ROM before starting.");
                return;
            }
 
            AppendLog("Starting emulation.");
            _startButton.Enabled = false;
            _stopButton.Enabled = true;
 
            _runTimer.Start();
        }
 
        private void OnStopClicked(object? sender, EventArgs e)
        {
            AppendLog("Stopping emulation.");
            _runTimer.Stop();
 
            _startButton.Enabled = true;
            _stopButton.Enabled = false;
        }
 
        private void OnResetClicked(object? sender, EventArgs e)
        {
            if (_emuHandle == IntPtr.Zero)
            {
                AppendLog("Cannot reset: emulator handle is null.");
                return;
            }
 
            bool ok = Native64Box.s64box_reset(_emuHandle);
            AppendLog(ok ? "Emulator reset." : "Emulator reset failed.");
        }
 
        private void OnRunTimerTick(object? sender, EventArgs e)
        {
            if (_emuHandle == IntPtr.Zero)
            {
                _runTimer.Stop();
                return;
            }
 
            const uint cyclesPerTick = 1000;
            bool ran = Native64Box.s64box_run_cycles(_emuHandle, cyclesPerTick);
            if (!ran)
            {
                AppendLog("s64box_run_cycles failed; stopping timer.");
                _runTimer.Stop();
                _startButton.Enabled = true;
                _stopButton.Enabled = false;
                return;
            }
 
            if (Native64Box.s64box_get_cpu_state(_emuHandle, out var cpu))
            {
                AppendLog(
                    $"AX={cpu.AX:X4} BX={cpu.BX:X4} CX={cpu.CX:X4} DX={cpu.DX:X4} " +
                    $"IP={cpu.IP:X4} CS={cpu.CS:X4}");
            }
            else
            {
                AppendLog("Failed to read CPU state.");
            }
 
            // Update text-mode display (80x25)
            const int cols = 80;
            const int rows = 25;
            var charsBuf = new byte[cols * rows];
 
            if (Native64Box.s64box_get_text_video(_emuHandle, charsBuf, null, cols, rows))
            {
                var sb = new StringBuilder();
                for (int y = 0; y < rows; y++)
                {
                    int rowStart = y * cols;
                    for (int x = 0; x < cols; x++)
                    {
                        byte ch = charsBuf[rowStart + x];
                        if (ch == 0)
                        {
                            ch = (byte)' ';
                        }
                        sb.Append((char)ch);
                    }
                    if (y < rows - 1)
                    {
                        sb.AppendLine();
                    }
                }
                _videoTextBox.Text = sb.ToString();
            }
        }
 
        private void AppendLog(string message)
        {
            var line = $"[{DateTime.Now:HH:mm:ss}] {message}";
            if (_logTextBox.TextLength == 0)
            {
                _logTextBox.Text = line;
            }
            else
            {
                _logTextBox.AppendText(Environment.NewLine + line);
            }
        }
    }
}