#include <RTRamp.h>
RTRamp::RTRamp(uint16_t ms_to_max, uint16_t range, uint16_t initValue)
{
  m_value = initValue;
  m_range = range;
  m_ms_to_max = ms_to_max;
  //m_period_micros = (m_ms_to_max * 1000) / m_range;
  m_step_timer = RTTimer((m_ms_to_max * 1000) / m_range);

}
void RTRamp::set(uint16_t pwm_value)
{
  m_target = pwm_value;
}
uint16_t RTRamp::get()
{
  return m_value;
}
void RTRamp::start()
{
  Serial.print("step : ");
  Serial.println(String((m_ms_to_max * 1000) / m_range));
  m_step_timer.async_reset();
  m_changed = true;
}

bool RTRamp::changed()
{
  return m_changed;
}
void RTRamp::run()
{
  m_changed = false;
  if(m_step_timer.isOver())
  {
    auto offset = m_step_timer.sync_reset();
    if(m_value < m_target)
    {
      if(m_value + offset < m_target)
      {
        m_value+=offset;
      }
      else
      {
        m_value = m_target;
      }
      m_changed = true;
    }
    else if(m_value > m_target)
    {
      if(offset > m_value)
      {
        m_value = m_target;
      }
      else if(m_value - offset > m_target)
      {
        m_value-=offset;
      }
      else
      {
        m_value = m_target;
      }
      m_changed = true;
    }
  }
}
