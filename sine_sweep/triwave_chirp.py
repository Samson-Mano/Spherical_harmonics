import matplotlib.pyplot as plt
import numpy as np
import math as math

class triwave_ChirpAnalysis:
    def __init__(self, f_start, f_end, f_m, f_phase, t_final, t_delta):
        # Initialize the data
        self.f_start = f_start
        self.f_end = f_end
        self.period = (1.0/ f_m) # frequency of the triangular function
        self.f_phase_preiod = (f_phase/(2.0*math.pi)) * self.period # phase period of the triangular function
        self.t_final = t_final
        self.t_delta = t_delta
        
        # Time data
        self.t_data = np.arange(0.0, t_final + t_delta, t_delta)

    def triwave_chirp_freq(self):
        # Chirp frequencies    
        t_half = self.period * 0.5
        freq_increase = (self.f_end - self.f_start) / t_half
        freq_decrease = (self.f_start - self.f_end) / t_half
        
        chirp_freq = np.zeros_like(self.t_data)
        self.chirp_phasefreq = np.zeros_like(self.t_data)

        cycle_count = 0

        for i, t in enumerate(self.t_data):
            
            # Find the cycle count
            if(((t + self.f_phase_preiod) - (cycle_count * self.period))>=self.period):
                cycle_count = cycle_count + 1

            # find the current cycle
            t_at = (t + self.f_phase_preiod) - (cycle_count * self.period)

            if t_at <= t_half:
                chirp_freq[i] = self.f_start + freq_increase * t_at
                self.chirp_phasefreq[i] = (self.f_start* t_at) + (0.5 * freq_increase * t_at* t_at)
            else:
                chirp_freq[i] = self.f_end + freq_decrease * (t_at - t_half)
                self.chirp_phasefreq[i] = (self.f_end*t_at )+ (0.5*freq_decrease *t_at*t_at)-(freq_decrease * t_half * t_at)
        return chirp_freq

    def triwave_chirp_signal(self):
        # Calculate the frequency sweep as a triangular function
        # t_half = self.t_final / 2.0
        # freq_increase = (self.f_end - self.f_start) / t_half
        # freq_decrease = (self.f_start - self.f_end) / t_half
        
        # chirp_freq = np.zeros_like(self.t_data)
        # for i, t in enumerate(self.t_data):
        #     if t <= t_half:
        #         chirp_freq[i] = self.f_start*t + 0.5*freq_increase * t*t
        #     else:
        #         chirp_freq[i] = (self.f_end*t )+ (0.5*freq_decrease *t*t)-(freq_decrease * t_half * t)


        lin_chirp = np.sin(2 * np.pi * self.chirp_phasefreq)
        return lin_chirp

# Plot the chirp frequencies
f_start = 1.0
f_end = 5.0
t_delta = 0.001
t_final = 30
f_m = 0.06
f_phase = math.pi*0.0


lin_chirp = triwave_ChirpAnalysis(f_start, f_end,f_m, f_phase, t_final, t_delta)
    
# Create a figure and 2 subplots
fig, axes = plt.subplots(nrows=2, ncols=1, figsize=(20, 8))

# Plot data on each subplot
axes[0].plot(lin_chirp.t_data, lin_chirp.triwave_chirp_freq(), color='blue')
axes[0].axhline(y=0.0, color='black', linestyle='-', linewidth=1)
axes[0].axvline(x=0.0, color='black', linestyle='-', linewidth=1)
axes[0].set_title('Chirp Frequencies')

axes[1].plot(lin_chirp.t_data, lin_chirp.triwave_chirp_signal(), color='green')
axes[1].axhline(y=0.0, color='black', linestyle='-', linewidth=1)
axes[1].axvline(x=0.0, color='black', linestyle='-', linewidth=1)
axes[1].set_title('Chirp Signal')

axes[-1].set_xlabel('Time (sec)')

# Set the main title for the figure
fig.suptitle('Triangular Chirp')

# Display the plot
plt.show()