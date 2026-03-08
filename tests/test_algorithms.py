"""
Unit tests for the pure algorithmic logic in lilygo firmware.

Tests run on the host (no hardware needed) by reimplementing the same
C algorithms in Python and verifying against known-good reference values.

Run:
    python3 tests/test_algorithms.py -v
"""

import math
import unittest

# ── Date helpers ──────────────────────────────────────────────────────────────

MONTH_NAMES = [
    "", "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December",
]

DOW_EN = ["Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"]
DOW_HI = ["शनिवार", "रविवार", "सोमवार", "मंगलवार", "बुधवार", "गुरुवार", "शुक्रवार"]


def zeller(year: int, month: int, day: int) -> int:
    """Zeller's congruence.  Returns h where 0=Sat, 1=Sun, …, 6=Fri."""
    m, y = month, year
    if m < 3:
        m += 12
        y -= 1
    k, j = y % 100, y // 100
    h = (day + 13 * (m + 1) // 5 + k + k // 4 + j // 4 - 2 * j) % 7
    return h % 7  # ensure positive


def build_date_info(year: int, month: int, day: int) -> dict:
    h = zeller(year, month, day)
    return {
        "dayName":  DOW_EN[h],
        "hindiDay": DOW_HI[h],
        "dateStr":  f"{MONTH_NAMES[month]} {day}, {year}",
    }


# ── Moon helpers ──────────────────────────────────────────────────────────────

SYN = 29.530588853
REF = 2451550.09765   # JDE of reference new moon (2000-Jan-06)

TITHI_S = ["Pratipada","Dwitiya","Tritiya","Chaturthi","Panchami",
           "Shashthi","Saptami","Ashtami","Navami","Dashami",
           "Ekadashi","Dwadashi","Trayodashi","Chaturdashi","Purnima"]
TITHI_K = ["Pratipada","Dwitiya","Tritiya","Chaturthi","Panchami",
           "Shashthi","Saptami","Ashtami","Navami","Dashami",
           "Ekadashi","Dwadashi","Trayodashi","Chaturdashi","Amavasya"]


def julian_day(year: int, month: int, day: int) -> int:
    """Integer Julian Day Number (noon) — mirrors C julianDay()."""
    y, m = year, month
    if m <= 2:
        y -= 1
        m += 12
    A = y // 100
    B = 2 - A + A // 4
    return int(365.25 * (y + 4716)) + int(30.6001 * (m + 1)) + day + B - 1524


def build_moon_info(year: int, month: int, day: int) -> dict:
    jd  = float(julian_day(year, month, day))
    age = math.fmod(jd - REF, SYN)
    if age < 0:
        age += SYN
    illum = int((1.0 - math.cos(2.0 * math.pi * age / SYN)) / 2.0 * 100.0 + 0.5)

    if   age <  1.85: phase = "New Moon"
    elif age <  7.38: phase = "Waxing Crescent"
    elif age <  9.22: phase = "First Quarter"
    elif age < 14.77: phase = "Waxing Gibbous"
    elif age < 16.61: phase = "Full Moon!"
    elif age < 22.15: phase = "Waning Gibbous"
    elif age < 24.0:  phase = "Last Quarter"
    else:             phase = "Waning Crescent"

    t_num = int(age / (SYN / 30.0))
    if t_num <= 14:
        paksha, tithi = "Shukla", TITHI_S[t_num]
    else:
        paksha, tithi = "Krishna", TITHI_K[t_num - 15]

    return {"age": age, "illum": illum, "phase": phase,
            "paksha": paksha, "tithi": tithi}


# ── Tests ─────────────────────────────────────────────────────────────────────

class TestJulianDay(unittest.TestCase):
    def test_known_j2000(self):
        # 2000-Jan-1.5 = JD 2451545.0; our function returns noon JD
        self.assertEqual(julian_day(2000, 1, 1), 2451545)

    def test_known_date(self):
        # 1858-Nov-17 midnight = JD 2400000.5; at noon = JD 2400001.
        # Our function returns the noon integer JD, so 2400001 is correct.
        self.assertEqual(julian_day(1858, 11, 17), 2400001)

    def test_march_2026(self):
        # Verify the JD used in moon calculation for project date
        jd = julian_day(2026, 3, 8)
        self.assertGreater(jd, 2461000)
        self.assertLess(jd, 2462000)


class TestZeller(unittest.TestCase):
    def test_sunday_mar8_2026(self):
        self.assertEqual(DOW_EN[zeller(2026, 3, 8)], "Sunday")

    def test_saturday_jan1_2000(self):
        self.assertEqual(DOW_EN[zeller(2000, 1, 1)], "Saturday")

    def test_monday(self):
        self.assertEqual(DOW_EN[zeller(2026, 3, 9)], "Monday")

    def test_friday(self):
        self.assertEqual(DOW_EN[zeller(2026, 3, 13)], "Friday")


class TestDateInfo(unittest.TestCase):
    def test_mar8_2026(self):
        d = build_date_info(2026, 3, 8)
        self.assertEqual(d["dayName"],  "Sunday")
        self.assertEqual(d["hindiDay"], "रविवार")
        self.assertEqual(d["dateStr"],  "March 8, 2026")

    def test_date_string_format(self):
        d = build_date_info(2025, 12, 25)
        self.assertEqual(d["dateStr"], "December 25, 2025")


class TestMoonPhase(unittest.TestCase):
    def test_mar8_2026_phase(self):
        """Known reference: Mar 8 2026 ≈ 19d, Waning Gibbous, Krishna Panchami."""
        m = build_moon_info(2026, 3, 8)
        self.assertAlmostEqual(m["age"], 19.0, delta=1.0)
        self.assertEqual(m["phase"], "Waning Gibbous")
        self.assertEqual(m["paksha"], "Krishna")
        self.assertEqual(m["tithi"], "Panchami")

    def test_illumination_bounds(self):
        for month in range(1, 13):
            m = build_moon_info(2026, month, 15)
            self.assertGreaterEqual(m["illum"], 0)
            self.assertLessEqual(m["illum"], 100)

    def test_new_moon_near_zero_illum(self):
        # 2026-Feb-17 is a new moon (age ≈ 0-1 d)
        m = build_moon_info(2026, 2, 18)
        self.assertLess(m["illum"], 5)
        self.assertEqual(m["phase"], "New Moon")

    def test_full_moon_near_100_illum(self):
        # 2026-Mar-3 is a full moon (age ≈ 14-15 d)
        m = build_moon_info(2026, 3, 3)
        self.assertGreater(m["illum"], 95)
        self.assertIn(m["phase"], ["Full Moon!", "Waxing Gibbous", "Waning Gibbous"])

    def test_age_in_range(self):
        for day in range(1, 30):
            m = build_moon_info(2026, 3, day)
            self.assertGreaterEqual(m["age"], 0)
            self.assertLess(m["age"], SYN)

    def test_tithi_shukla_paksha(self):
        # Days 1-15 of cycle → Shukla paksha
        m = build_moon_info(2026, 2, 22)  # waxing phase
        if m["age"] < SYN / 2:
            self.assertEqual(m["paksha"], "Shukla")

    def test_tithi_krishna_paksha(self):
        # Days 15-30 of cycle → Krishna paksha
        m = build_moon_info(2026, 3, 8)
        self.assertEqual(m["paksha"], "Krishna")


class TestMoonIcon(unittest.TestCase):
    """Verify the terminator formula used in drawMoonIcon() for key phases."""

    def _terminator_x(self, cx: float, half_chord: float, age: float) -> float:
        if age <= SYN / 2:
            return cx + half_chord * math.cos(math.pi * age / (SYN / 2))
        else:
            return cx + half_chord * math.cos(math.pi * (age - SYN / 2) / (SYN / 2))

    def test_new_moon_no_lit(self):
        """Age=0: waxing, term_x = cx + half_chord → lit region [xr,xr] = empty."""
        cx, r = 100, 28
        dy = 0
        half_chord = math.sqrt(r * r - dy * dy)
        xr = cx + half_chord
        term_x = self._terminator_x(cx, half_chord, 0.0)
        self.assertAlmostEqual(term_x, xr, places=3)

    def test_first_quarter_right_half(self):
        """Age=SYN/4: term_x = cx → right half lit."""
        cx, r = 100, 28
        half_chord = float(r)
        term_x = self._terminator_x(cx, half_chord, SYN / 4)
        self.assertAlmostEqual(term_x, cx, delta=1.0)

    def test_full_moon_fully_lit(self):
        """Age=SYN/2: waxing boundary, term_x = cx - half_chord → full disk lit."""
        cx, r = 100, 28
        half_chord = float(r)
        xl = cx - half_chord
        term_x = self._terminator_x(cx, half_chord, SYN / 2)
        self.assertAlmostEqual(term_x, xl, delta=1.0)

    def test_last_quarter_left_half(self):
        """Age=3*SYN/4: waning, term_x = cx → left half lit."""
        cx, r = 100, 28
        half_chord = float(r)
        term_x = self._terminator_x(cx, half_chord, 3 * SYN / 4)
        self.assertAlmostEqual(term_x, cx, delta=1.0)


if __name__ == "__main__":
    unittest.main(verbosity=2)
