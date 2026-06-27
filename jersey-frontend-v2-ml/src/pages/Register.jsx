import { useState } from 'react'
import { Link, useNavigate } from 'react-router-dom'
import { register } from '../api/client'
import { useAuth } from '../context/AuthContext'
import toast from 'react-hot-toast'
import { UserPlus } from 'lucide-react'

export default function Register() {
  const [form, setForm]       = useState({ name: '', username: '', password: '' })
  const [loading, setLoading] = useState(false)
  const { signIn }            = useAuth()
  const navigate              = useNavigate()

  const handleSubmit = async e => {
    e.preventDefault()
    setLoading(true)
    try {
      const res  = await register(form.username, form.password, form.name)
      const data = res.data.data
      signIn(data, data.token)
      toast.success('Account created!')
      navigate('/')
    } catch {
      toast.error('Username already taken')
    } finally {
      setLoading(false)
    }
  }

  return (
    <div style={{
      minHeight: '100vh', display: 'flex',
      alignItems: 'center', justifyContent: 'center',
      background: 'radial-gradient(ellipse at 40% 60%, rgba(181,255,77,0.04) 0%, transparent 60%)',
    }}>
      <div style={{ width: '100%', maxWidth: 400, padding: '0 24px' }}>
        <div style={{ textAlign: 'center', marginBottom: 40 }}>
          <h1 style={{ fontSize: '3rem' }}>CREATE<span className="accent">ACCOUNT</span></h1>
          <p className="muted" style={{ marginTop: 8, fontSize: '0.9rem' }}>Join the fan community</p>
        </div>

        <div className="card fade-up" style={{ padding: 32 }}>
          <form onSubmit={handleSubmit} style={{ display: 'flex', flexDirection: 'column', gap: 16 }}>
            {[
              { key: 'name',     label: 'Full Name',  placeholder: 'Tarun K',  type: 'text' },
              { key: 'username', label: 'Username',   placeholder: 'tarun',    type: 'text' },
              { key: 'password', label: 'Password',   placeholder: '••••••••', type: 'password' },
            ].map(({ key, label, placeholder, type }) => (
              <div key={key}>
                <label style={{ fontSize: '0.8rem', color: 'var(--text-secondary)', display: 'block', marginBottom: 6 }}>
                  {label}
                </label>
                <input className="input" type={type} placeholder={placeholder}
                  value={form[key]}
                  onChange={e => setForm(f => ({ ...f, [key]: e.target.value }))}
                  required />
              </div>
            ))}

            <button className="btn btn-primary" type="submit" disabled={loading}
              style={{ width: '100%', justifyContent: 'center', marginTop: 8, padding: '12px' }}>
              <UserPlus size={16} />
              {loading ? 'Creating account...' : 'Create Account'}
            </button>
          </form>
        </div>

        <p style={{ textAlign: 'center', marginTop: 20, fontSize: '0.85rem', color: 'var(--text-secondary)' }}>
          Already have an account? <Link to="/login" style={{ color: 'var(--accent)' }}>Sign in</Link>
        </p>
      </div>
    </div>
  )
}
