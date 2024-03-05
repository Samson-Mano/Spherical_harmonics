import matplotlib.pyplot as plt
import numpy as np

class triangular_ChirpAnalysis:
    def __init__(self, f_start, f_end, t_final, t_delta):
        # Initialize the data
        self.f_start = f_start
        self.f_end = f_end
        self.t_final = t_final
        self.t_delta = t_delta
        
        # Chirp rate
        self.c_rate = (f_end - f_start) / float(t_final)

        self.t_data = np.arange(0.0, t_final + t_delta, t_delta)

    def triangular_chirp_freq(self):
        # Chirp frequencies    
        t_half = self.t_final / 2.0
        freq_increase = (self.f_end - self.f_start) / t_half
        freq_decrease = (self.f_start - self.f_end) / t_half
        
        chirp_freq = np.zeros_like(self.t_data)
        for i, t in enumerate(self.t_data):
            if t <= t_half:
                chirp_freq[i] = self.f_start + freq_increase * t
            else:
                chirp_freq[i] = self.f_end + freq_decrease * (t - t_half)
        return chirp_freq

    def triangular_chirp_signal(self):
        # Calculate the frequency sweep as a triangular function
        t_half = self.t_final / 2.0
        freq_increase = (self.f_end - self.f_start) / t_half
        freq_decrease = (self.f_start - self.f_end) / t_half
        
        chirp_freq = np.zeros_like(self.t_data)
        for i, t in enumerate(self.t_data):
            if t <= t_half:
                chirp_freq[i] = self.f_start*t + 0.5*freq_increase * t*t
            else:
                chirp_freq[i] = (self.f_end*t )+ (0.5*freq_decrease *t*t)-(freq_decrease * t_half * t)


        lin_chirp = np.sin(2 * np.pi * chirp_freq)
        return lin_chirp

# Plot the chirp frequencies
f_start = 1.0
f_end = 5.0
t_delta = 0.001
t_final = 10

lin_chirp = triangular_ChirpAnalysis(f_start, f_end, t_final, t_delta)
    
# Create a figure and 2 subplots
fig, axes = plt.subplots(nrows=2, ncols=1, figsize=(20, 8))

# Plot data on each subplot
axes[0].plot(lin_chirp.t_data, lin_chirp.triangular_chirp_freq(), color='blue')
axes[0].axhline(y=0.0, color='black', linestyle='-', linewidth=1)
axes[0].axvline(x=0.0, color='black', linestyle='-', linewidth=1)
axes[0].set_title('Chirp Frequencies')

axes[1].plot(lin_chirp.t_data, lin_chirp.triangular_chirp_signal(), color='green')
axes[1].axhline(y=0.0, color='black', linestyle='-', linewidth=1)
axes[1].axvline(x=0.0, color='black', linestyle='-', linewidth=1)
axes[1].set_title('Chirp Signal')

axes[-1].set_xlabel('Time (sec)')

# Set the main title for the figure
fig.suptitle('Triangular Chirp')

# Display the plot
plt.show()