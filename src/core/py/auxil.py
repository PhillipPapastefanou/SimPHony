

def format_duration(seconds):
    m, s = divmod(seconds, 60)
    h, m = divmod(m, 60)
    return f'{h:02d}:{m:02d}:{s:02d}'

def remaining_str(seconds_elapsed, completed_sims,  total_sims):
    seconds = int((total_sims - completed_sims) * seconds_elapsed / completed_sims);
    return format_duration(seconds)
