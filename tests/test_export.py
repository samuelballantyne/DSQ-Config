import os
import tempfile
from pathlib import Path
import pytest

def export_files(base, show_message=True):
    if os.name != 'nt' and base.startswith('C:/'):
        if show_message:
            raise RuntimeError('Path Warning')
        return False
    bat_dir = Path(base) / 'bat'
    bat_dir.mkdir(parents=True, exist_ok=True)
    (bat_dir / 'game.bat').write_text('echo run')
    return True

def launch_game(base):
    if not export_files(base, False):
        return False
    bat_file = Path(base) / 'bat' / 'game.bat'
    return bat_file.exists()

def test_export_success():
    with tempfile.TemporaryDirectory() as d:
        assert export_files(d) is True
        assert Path(d, 'bat', 'game.bat').exists()

def test_export_failure_windows_path_on_linux():
    if os.name == 'nt':
        pytest.skip('Windows-specific check')
    assert export_files('C:/invalid', False) is False

def test_launch_game_aborts_on_failed_export():
    if os.name == 'nt':
        pytest.skip('Windows-specific check')
    assert launch_game('C:/invalid') is False
